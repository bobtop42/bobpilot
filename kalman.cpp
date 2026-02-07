#include "kalman.h"
#include "io.h"

void KALMAN::xPred(long int dt)
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

void KALMAN::pPred(long int dt)
{
  float dtf = static_cast<float>(dt);
  float32x4_t v1 = {dtf, dtf, dtf, dtf}; //v1 = dt
  float32x4_t v2 = {P[3][3], P[3][4], P[3][5], P[4][3]}; // v2 = p10
  float32x4_t v3 = vmulq_f32(v1, v2); // v3 = v2 * v1
  v2 = {P[0][3], P[0][4], P[0][5], P[1][3]}; // v2 = p4
  v2 = vaddq_f32(v2, v3); // v2 = v2 + v3
  
  P[0][3] = vgetq_lane_f32(v2, 0); //p4 = v2
  P[0][4] = vgetq_lane_f32(v2, 1);
  P[0][5] = vgetq_lane_f32(v2, 2);
  P[1][3] = vgetq_lane_f32(v2, 3);
  
  v2 = vmulq_f32(v2, v1);// v2 = v2 * v1
  float32x4_t v4 = {P[3][0], P[3][1], P[3][2], P[4][0]}; // v4 = p7
  v3 = vaddq_f32(v3, v4); //v3 = v3 + v4

  P[3][0] = vgetq_lane_f32(v2, 0); //p7 = v2
  P[3][1] = vgetq_lane_f32(v2, 1);
  P[3][2] = vgetq_lane_f32(v2, 2);
  P[4][0] = vgetq_lane_f32(v2, 3);

  v4 = vmulq_f32(v4, v1); // v4 = v4 * v1

  v3 = {P[0][0], P[0][1], P[0][2], P[1][0]}; // v3 = p1
  v3 = vaddq_f32(v3, v4); // v3 = v3 + v4
  v3 = vaddq_f32(v3, v2); // v3 = v3 + v2

  P[0][0] = vgetq_lane_f32(v3, 0); //p1 = v3
  P[0][1] = vgetq_lane_f32(v3, 1);
  P[0][2] = vgetq_lane_f32(v3, 2);
  P[1][0] = vgetq_lane_f32(v3, 3);



  
  v2 = {P[4][5], P[5][3], P[5][4], P[5][5]}; // v2 = p11
  v3 = vmulq_f32(v1, v2); // v3 = v2 * v1
  v2 = {P[1][5], P[2][3], P[2][4], P[2][5]}; // v2 = p5
  v2 = vaddq_f32(v2, v3); // v2 = v2 + v3

  P[1][5] = vgetq_lane_f32(v2, 0); //p5 = v2
  P[2][3] = vgetq_lane_f32(v2, 1);
  P[2][4] = vgetq_lane_f32(v2, 2);
  P[2][5] = vgetq_lane_f32(v2, 3);

  v2 = vmulq_f32(v2, v1); // v2 = v2 * v1
  v4 = {P[4][2], P[5][0], P[5][1], P[5][2]}; // v4 = p8
  v3 = vaddq_f32(v3, v4); // v3 = v3 + v4

  P[1][5] = vgetq_lane_f32(v2, 0); //p8 = v2
  P[2][3] = vgetq_lane_f32(v2, 1);
  P[2][4] = vgetq_lane_f32(v2, 2);
  P[2][5] = vgetq_lane_f32(v2, 3);

  v4 = vmulq_f32(v4, v1); // v4 = v4 * v1

  v3 = {P[1][2], P[2][0], P[2][1], P[2][2]}; // v3 = p2
  v3 = vaddq_f32(v3, v4); // v3 = v3 + v4
  v3 = vaddq_f32(v3, v2); // v3 = v3 + v2

  P[1][2] = vgetq_lane_f32(v3, 0); //p2 = v3
  P[2][0] = vgetq_lane_f32(v3, 1);
  P[2][1] = vgetq_lane_f32(v3, 2);
  P[2][2] = vgetq_lane_f32(v3, 3);

  

  v2 = {P[4][4]}; // v2 = p12
  v3 = vmulq_f32(v1, v2); // v3 = v2 * v1
  v2 = {P[1][4]}; // v2 = p6
  v2 = vaddq_f32(v2, v3); // v2 = v2 + v3

  P[1][4] = vgetq_lane_f32(v2, 0); //p6 = v2

  v2 = vmulq_f32(v2, v1); // v2 = v2 * v1
  v4 = {P[4][1]}; // v4 = p9
  v3 = vaddq_f32(v3, v4); // v3 = v3 + v4

  P[1][5] = vgetq_lane_f32(v2, 0); //p9 = v2
  P[2][3] = vgetq_lane_f32(v2, 1); 
  P[2][4] = vgetq_lane_f32(v2, 2);
  P[2][5] = vgetq_lane_f32(v2, 3);

  v4 = vmulq_f32(v4, v1); // v4 = v4 * v1

  v3 = {P[1][2], P[2][0], P[2][1], P[2][2]}; // v3 = p3
  v3 = vaddq_f32(v3, v4); // v3 = v3 + v4
  v3 = vaddq_f32(v3, v2); // v3 = v3 + v2

  P[1][2] = vgetq_lane_f32(v3, 0); //p3 = v3
  P[2][0] = vgetq_lane_f32(v3, 1);
  P[2][1] = vgetq_lane_f32(v3, 2);
  P[2][2] = vgetq_lane_f32(v3, 3);
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
  xPred(plane->dt);
  pPred(plane->dt);
  kGain();
  measureUpdate(plane);
  updateState();
  updateP(); 
  pushKalmandData(plane);
}
