#include "navsys.h"
//#include "kalman.h"

//READ ME: num of wp vals set in main? 14 for rn tho

NAVSYS::NAVSYS():npc(0), routeLen(14) {};

//wayPointAngleFinder has been updated! Nww completely branchless. yay. 
void NAVSYS::wayPointAngleFinder(PLANE* plane)
{
  float wx = plane->planeft.wxft;
  float wy = plane->planeft.wyft;;
  float wz = plane->planeft.wzft;

  float px = plane->planeft.pxft;
  float py = plane->planeft.pyft;
  float pz = plane->planeft.pzft;

  wy-=py; wx-=px; wz-=pz;

  plane->WPA.roll = bfabs(batan2(wx,wz)) + ((wx/bfabs(wx) - 1.0f) * -1.570795f) + ((wz/bfabs(wz) - 1.0f) * -0.7853795);
  plane->WPA.pitch = batan2(wy, (qsqrt(wx * wx + wz * wz)));
};

//fill in wiht kalman filter stuff later: maybevoid NAVSYS::planeAngleFinder(KALMAN* kalman)???
//also planePointAngleFinder has been updated! Nww completely branchless. yay.
void NAVSYS::planeAngleFinder(CKALMAN* ckalman, PLANE* plane, HMC::HMC* hmc)
{
  float x = plane->pAngle[0][0];
  float y = plane->pAngle[1][0];
  float z = plane->pAngle[2][0];

  plane->PA.roll = bfabs(batan2(x, z) + ((x/bfabs(x) - 1.0f) * -1.570795f) + ((z/bfabs(z) - 1.0f) * -0.7853795));
  plane->PA.pitch = batan2(y, (qsqrt(x * x + z * z)));

  ckalman->loop(plane, hmc);
  plane->PA.roll = ckalman->returnX(0);
  
}

void NAVSYS::updateEP(PLANE& plane, float value)
{
  plane.ep.flap.fR = plane.ep.flap.fL += value;
  plane.ep.flapAdj(plane.ep);
}

void NAVSYS::updateAP(PLANE& plane, float value)
{
  plane.ap.flap.fR = plane.ap.flap.fL += value;
  plane.ap.flapAdj(plane.ap);
}

void NAVSYS::updateNpc(PLANE* plane)
{
  px = plane->planeft.pxft; py = plane->planeft.pyft; pz = plane->planeft.pzft;
  wx = plane->planeft.wxft; wy = plane->planeft.wyft; wz = plane->planeft.wzft;

  wx = bfabs(wx - px); wy = bfabs(wy - py); wz = bfabs(wz - pz);

  /*plane must be withing ft of the waypoint to be counted as marked. code below dis, then truncates the flip val with !*/

  uintptr_t wpIncra = (!static_cast<uintptr_t>(wx/6.0f)) *  
                      (!static_cast<uintptr_t>(wy/6.0f)) *
                      (!static_cast<uintptr_t>(wz/6.0f));

  uintptr_t wpaddr = (reinterpret_cast<uintptr_t>(plane->WPXYZ.nextWPpos->next_)* wpIncra) + (reinterpret_cast<uintptr_t>(plane->WPXYZ.nextWPpos)* (!wpIncra));

  plane->WPXYZ.nextWPpos = reinterpret_cast<WPROUTE<int>::WP*>(wpaddr);
  
  //if true npc==routelen==0, then flips with ! 2x(zero -> 1, non-zero -> 0)
  routeCompleted = static_cast<uint8_t>(!wpaddr);
}

/*
void NAVSYS::setUpRoute()
{
  std::cout<<"enter a number of waypoints for the route: \n";
  std::cin>>routeLen;

  float WPROUTEPOINTERS[this->routeLen][3] = new float;
}
*/
