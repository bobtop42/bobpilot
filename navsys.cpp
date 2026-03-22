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

void NAVSYS::errorCalc(PLANE *plane)
{
foriunion pxv; pxv.f = plane->pAngle[0][0];
foriunion pyv; pyv.f = plane->pAngle[0][1];
foriunion pzv; pzv.f = plane->pAngle[0][2];

uint8_t flags = !(pxv.i & 0x7FFFFFFF);

pxv.i = (!(pxv.i & 0x7FFFFFFF) * 0x3F800000) | ((pxv.i & 0x7FFFFFFF) * pxv.i);
float rslope = pzv.f/pxv.f;

//LOOK INTO SLOPE CALC!!!
float t1, t2; foriunion t3;
t1 = (pxv.f * 0.0001f); t1 *= t1;
t2 = (pzv.f * 0.0001f); t2 *= t2;

t3.f = qsqrt(t1 + t2);
flags |= (!(t3.i & 0x7FFFFFFF)) <<1;
t3.i = (!(t3.i & 0x7FFFFFFF) * 0x3F800000) | ((t3.i & 0x7FFFFFFF) * t3.i);
t2 = t3.f;

t3.f = (pyv.f * 0.0001f); t3.f *= t3.f;

float pslope = (t3.f / t2) * 100.0f;

foriunion  x; foriunion y ; foriunion  z;
foriunion rx;               foriunion rz;
foriunion px; foriunion py;

x.f = (plane->planeft.pxft - plane->planeft.wxft) * 0.001f; 
y.f = (plane->planeft.pyft - plane->planeft.wyft) * 0.001f; 
std::cout<<"y.f: "<<y.f<<"\n";
z.f = (plane->planeft.pzft - plane->planeft.wzft) * 0.001f; 

float rradius = qsqrt(x.f * x.f + z.f * z.f);

float u, v, w;
u = rradius * 0.001f;
v = y.f * 0.001f;

float pradius = qsqrt(u + v) * 31.62278f;

foriunion ra; foriunion pa;
ra.f = rradius / qsqrt(rslope * rslope + 1.0f);
pa.f = pradius / qsqrt(pslope * pslope + 1.0f);

float ratx = static_cast<float>(!(flags & 0x1));
float ratz = static_cast<float>(flags & 0x1) * rradius;

//add +/- inf det. and make rz = rradius
rx.f = ra.f * ratx; rz.f = ra.f * rslope;
rz.f = ratx * rz.f + ratz;

float patx = static_cast<float>(!(flags & 0x04)); //add all flags correctly to variable
float paty = static_cast<float>(!!(flags & 0x04)) * pradius;

px.f = pa.f * patx; py.f = pa.f * pslope;
py.f = patx * py.f + paty;

px.i = (px.i & 0x7FFFFFFF) | (static_cast<int32_t>(plane->pAngle[0][0]) & signbit32);
rx.i = (rx.i & 0x7FFFFFFF) | (static_cast<int32_t>(plane->pAngle[0][0]) & signbit32);
py.i = (py.i & 0x7FFFFFFF) | (static_cast<int32_t>(plane->pAngle[0][1]) & signbit32);
rz.i = (rz.i & 0x7FFFFFFF) | (static_cast<int32_t>(plane->pAngle[0][2]) & signbit32);

foriunion rerror; foriunion perror;

rerror.f = rx.f * rslope; perror.f * px.f;
rerror.f -= rz.f; rerror.i = (rerror.i & signbit32) | 0x3F800000;
perror.f -= py.f; perror.i = (perror.i & signbit32) | 0x3F800000;

perror.i ^= px.i & signbit32;
rerror.i ^= rx.i & signbit32;

t1 = x.f - rx.f; t2 = z.f - rz.f;
rerror.f = qsqrt(t1 * t1 + t2 * t2);
rerror.f = (rerror.f / rradius) * (1.0f / rradius);

t1 = x.f - px.f; t2 = y.f - py.f;
perror.f = qsqrt(t1 * t1 + t2 * t2);
perror.f = (perror.f / pradius) * (1.0f / pradius);

plane->error.roll = rerror.f;
plane->error.pitch = perror.f;
}
