#ifndef PLANE_H
#define PLANE_H
#include "MATHLIB.h"
#include "ckalman.h"

// Define missing types

bool SHUTDOWNERROR = false;

void shutDownErrorCheck()
{
  if(SHUTDOWNERROR)
  {
    std::cout << "SHUTDOWN ERROR" << std::endl;
    //add code to return to main/realio stuff to hand over control to a real user
  }
}

class PLANE
{
public:

//waypoint data for gps
angle WPA;
pt3D loc;
float WPXYZ[14][3];
//plane data from MPU 6050
angle PA;
float pAngle[3][3];

EP ep;
AP ap;
//aspeed 
float speed;
Time time;
int npc;
int gpsAcc;
HEMISPHERE hemisphere;

float AC[3];

CKALMAN ckalman;
HMC::HMC hmc;

void updateGPS(float lat, float alt, float Long, char hem1, char hem2, float Acc, int hrs, int min, int sec)
{
  loc.x = lat;
  loc.y = alt;
  loc.z = Long;
  hemisphere.NS = hem1;
  hemisphere.EW = hem2;
  gpsAcc = Acc;
  time.updateTime(hrs, min, sec, this);
}

};

#endif
