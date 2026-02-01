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
NOTE: The NAVSYS class is currently being redone in order to be more efficient. When completed, NAVSYS will no longer use trig. functions but rater a more linearized approach to calculating error. This means that the new system will not be usng radians or degrees to quatify errors. If anyone reading this has any suggestions please let me know.

Moreover, other parts of the the AP are also being modified to work with the new system. Basically the ROLL, PITCH, and ATC classes are being "re-plumbed". Additionally the HOLD class will also see significant changes as well.

I will temptorary be leaving the old code in place for now, so progression can be watched, or whatever the people who view this code do, can see the changes too, and not be shocked with one big update.
*/
void NAVSYS::errorCalc(PLANE *plane)
{
px = plane->pAngle[0][0];
py = plane->pAngle[0][1];
pz = plane->pAngle[0][2];

float rplaneSlope = px/pz;

foriunion x; foriunion y; foriunion z;

x.f = longToFeet( plane->loc.z, plane->loc.x) - plane->planeft.wxft;
y.f = plane->loc.y - plane->planeft.wyft;
z.f = latToFeet( plane->loc.z) - plane->planeft.wzft;

float rradius = qsqrt(x.f * x.f + z.f * z.f);
float rplaneSlope2 = rplaneSlope * rplaneSlope;
float rradius2 = rradius * rradius;

float rx2z2 = qsqrt( px * px + pz * pz);
float pplaneSlope = py / rx2z2;
float pplaneSlope2 = pplaneSlope * pplaneSlope;
float pradius = qsqrt( rradius2 + y.f * y.f);
float pradius2 = pradius * pradius;

float32x2_t vr = {rradius2, pradius2};
float32x2_t vs = {rplaneSlope2, pplaneSlope2};
float32x2_t vh = vdup_n_f32(0.5f);; /*fill in with the vector fill cmd*/

vs = vrecipq_f32(vs);
vr = vmulq_f32(vr, vs);
vr = vmulq_f32(vr, vh);

rradius2 = vgetq_lane_f32(vr, 0);
pradius2 = vgetq_lane_f32(vr, 1);

foriunion ra; foriunion pa;
ra.f = qsqrt(rradius2); pa.f = qsqrt(pradius2);

px = ra.f; pz = ra.f * rplaneSlope;
py = pa.f * pplaneSlope; /*check the mul*/

float rwpSlope = z.f/x.f; float pwpSlope = y.f/rradius;

ra.f *= rwpSlope; pa.f *= pwpSlope;
ra.f -= px; ra.i &= 0x80000000;
pa.f -= rx2z2; pa.i &= 0x80000000;

px -= plane->planeft.wxft;
py -= plane->planeft.wyft;
pz -= plane->planeft.wzft;

foriunion rerror; foriunion perror;
rerror.f = qsqrt(px * px + pz * pz) / (2.0f *rradius);
perror.f = qsqrt(py * py + rradius * rradius) / (2.0f *pradius);

rerror.i |= ra.i ^ (x.i & 0x80000000);
x.f = rradius; //check this line
perror.i |= pa.i ^ (x.i & 0x80000000); //check this line too

plane->error.roll = rerror.f;
plane->error.pitch = perror.f;
/*look into weighing the errors*/
}
