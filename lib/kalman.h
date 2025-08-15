#include "i2c.h"
#ifndef KALMAN_H
#define KALMAN_H 

class KALMAN
{
public:
float X[6][1];

KALMAN();

float setUp();
void xPred(float (&X)[6][1]);
void pPred(float (&P)[6][6]);
void kGain(float (&K)[6][3]);
void measureUpdate(float (&pAngle)[3][3]);
void updateState(float (&X)[6][1]);
void updateP(float (&P)[6][6]);
void loop(float (&X)[6][1], float (&P)[6][6], float (&K)[6][3], float (&Y)[6][1]);

private:
enum{MK = 6, MX = 1, MB = 3};

bool setUp_ = false;



float Y[6][1];
float K[6][6];

const float A[3][3] = 
{
  {1.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f}
};

const float dt[3][3] =
{
  {0.1f, 0.0f, 0.0f},
  {0.0f, 0.1f, 0.0f},
  {0.0f, 0.0f, 0.1f}
};

const float P[6][6] = 
{
  {0.0987f, 0.1381f, 0.1165f, 0.0888f, 0.0987f, 0.1381f,},
  {0.1381f, 0.1934f, 0.1632f, 0.1243f, 0.1381f, 0.1934f,},
  {0.1165f, 0.1632f, 0.1376f, 0.1049f, 0.1165f, 0.1632f,},
  {0.0888f, 0.1243f, 0.1049f, 0.0799f, 0.0888f, 0.1243f,},
  {0.0987f, 0.1381f, 0.1165f, 0.0888f, 0.0987f, 0.1381f,},
  {0.1381f, 0.1934f, 0.1632f, 0.1243f, 0.1381f, 0.1934f,},
};
const float R[6][6] = {
  {0.00974f, 0.01907f, 0.01358f, 0.00788f, 0.00974f, 0.01907f,},
  {0.01907f, 0.03736f, 0.02662f, 0.01546f, 0.01907f, 0.03736f,},
  {0.01358f, 0.02662f, 0.01894f, 0.01101f, 0.01358f, 0.02662f,},
  {0.00788f, 0.01546f, 0.01101f, 0.00639f, 0.00788f, 0.01546f,},
  {0.00974f, 0.01907f, 0.01358f, 0.00788f, 0.00974f, 0.01907f,},
  {0.01907f, 0.03736f, 0.02662f, 0.01546f, 0.01907f, 0.03736f,},
};

float AC[3][1] = 
{
  {0.0f},
  {0.0f},
  {0.0f}
};
//may need to change nts
const float B[6][3] = 
{
  {1.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f},
  {0.1f, 0.0f, 0.0f},   
  {0.0f, 0.1f, 0.0f},
  {0.0f, 0.0f, 0.1f}
};
};

#endif
