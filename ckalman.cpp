#include "ckalman.h"

void CKALMAN::xPred(float (&x)[2][1])
{
  //fill in with other matrixis later
  x[0][0] = x[0][0] + 0.1f * x[1][0];
  x[1][0] = x[1][0]; // check this
}

void CKALMAN::pPred(float (&p)[2][2])
{
  p[1][0] += 0.1f * p[1][1] + q[1][0];
  p[0][0] += 0.1f * (p[0][1] + p[1][0]) + q[0][0];
  p[0][1] += 0.1f * p[1][1] + q[0][1];
  p[1][1] += q[1][1];
}

void CKALMAN::kGain(float (&k)[2][2])
{
  k[1][0] = (p[1][0]/(p[0][0] + r[0][0])) + (p[1][1]/(p[1][0] + r[1][0]));
  k[0][0] = (p[0][0]/(p[0][0] + r[0][0])) + (p[0][1]/(p[1][0] + r[1][0]));
  k[0][1] = (p[0][0]/(p[0][1] + r[0][1])) + (p[0][1]/(p[1][1] + r[1][1]));
  k[1][1] = (p[1][0]/(p[0][1] + r[0][1])) + (p[1][1]/(p[1][1] + r[1][1]));
}

double magNorthOffset(PLANE* plane)
{
  char NS = plane->hemisphere.NS;
  char EW = plane->hemisphere.EW;

  int temp = static_cast<int>(plane->loc.x/40.0f);

  float temp2 = (static_cast<int>(!(('W'-EW)+temp)>>1));
  float mod = 139.298f-(98.596f*(static_cast<float>(temp2)));

  int addOrSub = (((static_cast<int>((!('W'-EW)<<1))+!temp)/3)<<1)-1;

  float NSmod = static_cast<float>(!('S'-NS))*85.762f;

  float px = plane->loc.x +(mod*static_cast<float>(addOrSub));
  float pz = plane->loc.z + NSmod; //adust for any lat deg and hemisphere(i believe)

  px = longToFeet(px, pz);
  pz = latToFeet(pz);

  return atan2(px, pz);
}

void CKALMAN::measureUpdate(float (&y)[2], PLANE* plane, HMC::HMC* hmc)
{
  //dont forget to add the z^m matrix at some point
  y[0] = plane->PA.roll;
  y[1] = static_cast<float>(hmc->heading(plane));
  y[1]+=static_cast<float>(magNorthOffset(plane));
}

void CKALMAN::xUpdate(float (&x)[2][1])
{
  y[0] -= x[0][0];
  y[1] -= x[1][0];
  //check below
  x[0][0] += (k[0][0] * y[0] + k[0][1] * y[1]) * 2.0f;
  x[1][0] += (k[1][0] * y[0] + k[1][1] * y[1]) * 2.0f;
}

void CKALMAN::updateP(float (&p)[2][2])
{
  i[0][0] -= k[0][0]; k[0][0] = p[0][0];
  i[0][1] -= k[0][1]; k[0][1] = p[0][1];
  i[1][0] -= k[1][0]; k[1][0] = p[1][0];
  i[1][1] -= k[1][1]; k[1][1] = p[1][1];

  p[0][0] = i[0][0] * k[0][0] + i[0][1] * k[1][0];
  p[0][1] = i[0][0] * k[0][1] + i[0][1] * k[1][1];
  p[1][0] = i[1][0] * k[0][0] + i[1][1] * k[1][0];
  p[1][1] = i[1][0] * k[0][1] + i[1][1] * k[1][1];
}

void CKALMAN::loop(PLANE* plane, HMC::HMC* hmc)
{
  //compass kalman filter loop
  xPred(x);
  pPred(p);
  kGain(k);
  measureUpdate(y, plane, hmc);
  xUpdate(x);
  updateP(p);
}


float CKALMAN::returnX(uint pos) { return x[((pos & 0x03)^0x01)>>1][0];}
