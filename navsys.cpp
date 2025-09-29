#include "navsys.h"
//#include "kalman.h"

//READ ME: num of wp vals set in main? 14 for rn tho
void NAVSYS::setRouteLen()
{
  routeLen = 14;
}

NAVSYS::NAVSYS():npc(0), routeLen(14) {};

//wayPointAngleFinder has been updated! Nww completely branchless. yay. 
void NAVSYS::wayPointAngleFinder()
{
  float wx  = plane.WPXYZ[plane.npc][0];
  float wy = plane.WPXYZ[plane.npc][1];
  float wz = plane.WPXYZ[plane.npc][2];

  float px = plane.loc.x;
  float py = plane.loc.y;
  float pz = plane.loc.z;

  wx = longToFeet(wx, wz); 
  px = longToFeet(px, pz);
  
  wy = latToFeet(wx); 
  py = latToFeet(px);

  wy-=py; wx-=px; wz-=pz;

  plane.WPA.roll = fabs(atan2(wx,wz)) + ((wx/fabs(wx) - 1.0f) * -1.570795f) + ((wz/fabs(wz) - 1.0f) * -0.7853795);
  plane.WPA.pitch = atan2(wy, (sqrt(wx * wx + wz * wz)));
};

//fill in wiht kalman filter stuff later: maybevoid NAVSYS::planeAngleFinder(KALMAN* kalman)???
//also planePointAngleFinder has been updated! Nww completely branchless. yay.
void NAVSYS::planeAngleFinder(CKALMAN* ckalman, PLANE* plane, HMC::HMC* hmc)
{
  float x = plane->pAngle[0][0];
  float y = plane->pAngle[1][0];
  float z = plane->pAngle[2][0];

  plane->PA.roll = fabs(atan2(x, z) + ((x/fabs(x) - 1.0f) * -1.570795f) + ((z/fabs(z) - 1.0f) * -0.7853795));
  plane->PA.pitch = atan2(y, (sqrt(x * x + z * z)));

  ckalman->loop(plane, hmc);
  plane->PA.roll = ckalman->returnX(0);
  
}

void NAVSYS::updateEP(PLANE& plane, float value)
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

void NAVSYS::updateNpc(PLANE* plane)
{
  float wx = plane->WPXYZ[plane->npc][0];
  float wy = plane->WPXYZ[plane->npc][1];
  float wz = plane->WPXYZ[plane->npc][2];

  wx = longToFeet(wx, wz); wz = latToFeet(wz);

  float px = plane->loc.x;
  float py = plane->loc.y;
  float pz = plane->loc.z;

  px = longToFeet(px, pz); pz = latToFeet(pz);

  wx = fabs(wx-px); wy = fabs(wy-py); wz = fabs(wz-pz);

  //plane must be withing 4ft of the waypoint to be counted as marked. code below dis, then truncates the flip val with !
  npc += 1 * (!static_cast<int> (wx/4.0f)) *
             (!static_cast<int> (wy/4.0f)) *  
             (!static_cast<int> (wz/4.0f));
  //if true npc==routelen==0, then flips with ! 2x(zero -> 1, non-zero -> 0)
  routeCompleted = !!(routeLen - npc);
  
}
