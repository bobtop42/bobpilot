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

void CKALMAN::measureUpdate(float (&y)[2], PLANE* plane, HMC::HMC* hmc)
{
  //dont forget to add the z^m matrix at some point
  y[0] = plane->PA.roll;
  y[1] = static_cast<float>(hmc->heading(plane)); 
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
