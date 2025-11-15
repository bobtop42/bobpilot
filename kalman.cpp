#include "kalman.h"
#include "io.h"

void KALMAN::xPred()
{
  float AX[6];
  float BU [6];

  for(int i=0; i<3; ++i)
    {
      for(int j=0; j<3; ++j)
        {
          AX[i] = (A[i][j] * X[i][0])  + (A[i][j] * X[i+3][0]);
          AX[i+3] = (A[i][j] * X[i+3][0]);

          BU[i] += B[i][j] * AC[i][0];
          BU[i+3] += B[i+3][j] * AC[i][0];
        }

      X[i][0] = AX[i] + BU[i];
      X[i+3][0] = AX[i+3] + BU[i+3];
    }
}

void KALMAN::pPred()
{
  for(int i=0; i<3; ++i)
    {
      for(int j=0; j<3; ++j)
        {
          //new code below. very very very fast
          P[i+3][j]+=0.1f*P[i+3][j+3]; //add q matrix
          P[i][j]+=0.1f*(P[i][j+3]+P[i+3][j]);
          P[i][j+3]+=0.1f*P[i+3][j+3];
        }
    }
}

void KALMAN::kGain()
{
  float temp[6][6];

  for(int i=0; i<3; ++i)
    {
      for(int j=0; j<3; ++j)
        {
          temp[i][j] = P[i][j] + R[i][j];
          temp[i][j+3] = P[i][j+3] + R[i][j+3];
          temp[i+3][j] = P[i+3][j] + R[i+3][j];
          temp[i+3][j+3] = P[i+3][j+3] + R[i+3][j+3];

          K[i][j] = P[i][j]/temp[j][i];
          K[i][j+3] = P[i][j+3]/temp[j][i+3];
          K[i+3][j] = P[i+3][j]/temp[j+3][i];
          K[i+3][j+3] = P[i+3][j+3]/temp[j+3][i+3];
        }
    }
}

void KALMAN::measureUpdate(PLANE* plane)
{
  Y[0][0] = plane->pAngle[0][0];
  Y[1][0] = plane->pAngle[2][0];
  Y[2][0] = plane->pAngle[0][1];
  Y[3][0] = plane->pAngle[2][1];
  Y[4][0] = plane->pAngle[0][1];
  Y[5][0] = plane->pAngle[2][2];
}

void KALMAN::updateState()
{
  int temp[6];

  for(int i=0; i<3; ++i)
    {
      Y[i][0] -= X[i][0];
      Y[i+3][0] -= X[i+3][0];

      for(int j=0; j<3; ++j)
        {
          temp[i] += (K[i][j] * Y[j][0]) + (K[i][j+3] * Y[j+3][0]);
          temp[i+3] += (K[i+3][j] * Y[i][0]) + (K[i+3][j+3] * Y[i+3][0]);

          X[i][0] = X[i][0] + temp[i];
          X[i+3][0] = X[i+3][0] + temp[i+3];
        }
      
    }
}

void KALMAN::updateP()
{
  for(int i=0; i<3; ++i)
    {
      for(int j=0; j<3; ++j)
        {
          P[i][j] = ((1 - K[i][j]) * P[i][j]);
          P[i][j+3] = ((1 - K[i][j+3]) * P[i][j+3]);
          P[i+3][j] = ((1 - K[i+3][j]) * P[i+3][j]);
          P[i+3][j+3] = ((1 - K[i+3][j+3]) * P[i+3][j+3]);
        }
    }
}

void KALMAN::pushKalmandData(PLANE* plane)
{
  plane->pAngle[0][0] = Y[0][0];
  plane->pAngle[2][0] = Y[1][0];
  plane->pAngle[0][1] = Y[2][0];
  plane->pAngle[2][1] = Y[3][0];
  plane->pAngle[0][1] = Y[4][0];
  plane->pAngle[2][2] = Y[5][0];
}

void KALMAN::loop(PLANE* plane)
{
  xPred();
  pPred();
  kGain();
  measureUpdate(plane);
  updateState();
  updateP(); 
  pushKalmandData(plane);
}
