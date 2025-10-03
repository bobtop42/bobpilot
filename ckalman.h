#ifndef CKALMAN_H //compass kalman filter
#define CKALMAN_H

#include "plane.h"
#include "hmc.h"

class CKALMAN
{
private:
void xPred(float (&x)[2][1]);
void pPred(float (&p)[2][2]);
void kGain(float (&k)[2][2]);
void measureUpdate(float(&y)[2], PLANE* plane, HMC::HMC* hmc);
void xUpdate(float (&x)[2][1]);
void updateP(float (&p)[2][2]);

public:
void loop(PLANE* plane, HMC::HMC* hmc);
float returnX(uint pos);

private:
float x[2][1];
float p[2][2];
float k[2][2];
float y[2];
float i[2][2] =
{
  {1.0f, 1.0f},
  {1.0f, 1.0f}
};
float q[2][2];
float r[2][2];

};
  
#endif
