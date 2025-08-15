#include "navsys.h"
//#include "kalman.h"

//READ ME: num of wp vals set in main? 14 for rn tho
void flapPos::rad(flapPos& myFlapPos)
{
  fL = (myFlapPos.fL / 360.0f);
  fR = (myFlapPos.fR / 360.0f);
};

void EP::elevatorAdj(EP ep)
{
  if(ep.elevator.fL > 60.0f)
  {
    ep.elevator.fL = 60.0f;
    ep.elevator.fR = 60.0f;
    return;
  }
  else if(ep.elevator.fL < -60.0f)
  {
    ep.elevator.fL = -60.0f;
    ep.elevator.fR = -60.0f;
  }
};

void AP::aileronAdj(AP ap)
{
  if(ap.aileron.fL > 60.0f)
  {
    ap.aileron.fL = 60.0f;
    ap.aileron.fR = -60.0f;
    return;
  }
  else if(ap.aileron.fL < -60.0f)
  {
    ap.aileron.fL = -60.0f;
    ap.aileron.fR = 60.0f;
  }
};

void NAVSYS::wayPointAngleFinder()
{
  float wx = WPXYZ[npc][0];
  float wy = WPXYZ[npc][1];
  float wz = WPXYZ[npc][2];

  float px = plane.loc.x;
  float py = plane.loc.y;
  float pz = plane.loc.z;

  
  float y;

  if(wy < py) { y = py - wy; } else { y = wy - py; };

  if(wx < px && wz > pz)
  {
    float x = px - wx; float z = wz - pz;
    
    plane.WPA.roll = static_cast<float>(atan2(x, z));
    double sqrtXZ = (x*x + z*z);
    sqrtXZ = static_cast<float>(sqrt(sqrtXZ));
    plane.WPA.pitch = static_cast<float>(atan2(y, sqrtXZ));
    
    return;
  }
  else if(wx < px && wz < pz)
    {
      float x = px - wx; float z = pz - wz;

      plane.WPA.roll = static_cast<float>(atan2(x, z));
      float sqrtXZ = (x*x + z*z);
      sqrtXZ = static_cast<float>(sqrt(sqrtXZ));
      plane.WPA.pitch = static_cast<float>(atan2(y, sqrtXZ));
      
      return;
    }
  else if(wx > px && wz < pz)
    {
      float x = wx - px; float z = pz - wz;
      
      plane.WPA.roll = static_cast<float>(atan2(x, z));
      float sqrtXZ = (x*x + z*z);
      sqrtXZ = static_cast<float>(sqrt(sqrtXZ));
      plane.WPA.pitch = static_cast<float>(atan2(y, sqrtXZ));
      
      return;
    }
  else if(wx > px && wz > pz)
    {
      float x = wx - px; float z = wz - pz;
      
      plane.WPA.roll = static_cast<float>(atan2(x, z));
      float sqrtXZ = (x*x + z*z);
      sqrtXZ = static_cast<float>(sqrt(sqrtXZ));
      plane.WPA.pitch = static_cast<float>(atan2(y, sqrtXZ));

      return;
    }
};

//fill in wiht kalman filter stuff later: maybevoid NAVSYS::planeAngleFinder(KALMAN* kalman)???
void NAVSYS::planeAngleFinder(float kX[3][0])
{
  float x = kX[1][0];
  float y = kX[2][0];
  float z = kX[3][0];


  //comment out PAE counter stuff. its part of error handling.

  if(x < 0.0f && z > 0.0f)
  {
    plane.PA.roll = static_cast<float>(atan(x/z)*(-1.0f)) * (-1.0f);
    float sqrtXZ = (x*x + z*z);
    sqrtXZ = static_cast<float>(atan2(y, sqrtXZ));
    return;
  }
  else if(x >0.0f && z > 0.0f)
  {
    plane.PA.roll = static_cast<float>(atan(x/z)*(-1.0f)) * (-1.0f);
    float sqrtXZ = (x*x + z*z);
    sqrtXZ = static_cast<float>(atan2(y, sqrtXZ));
    return;
  }
  else if(x < 0.0f && z < 0.0f)
  {
    plane.PA.roll = static_cast<float>(atan(x/z) + (0.5f*PI));
    float sqrtXZ = (x*x + z*z);
    sqrtXZ = static_cast<float>(atan2(y, sqrtXZ));
    return;
  }
  else if( x > 0.0f && z < 0.0f)
  {
    plane.PA.roll = static_cast<float>(atan(x/z) + (0.5f*PI));
    float sqrtXZ = (x*x + z*z);
    sqrtXZ = static_cast<float>(atan2(y, sqrtXZ));
    return;
  }
    
  
}

void NAVSYS::updateEP(Plane& plane, float value)
{
  plane.ep.elevator.fL += value;
  plane.ep.elevator.fR += value;
  plane.ep.elevatorAdj(plane.ep);
}

void NAVSYS::updateAP(float value)
{
  if(value < PI && value > 0.0f)
  {
    plane.ap.aileron.fL -= value;
    plane.ap.aileron.fR += value;
    return;
  }
  else
  {
    plane.ap.aileron.fL += value;
    plane.ap.aileron.fR -= value;
  }
  
  plane.ap.aileronAdj(plane.ap);
  
}
