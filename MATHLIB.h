#ifndef MATHLIB_H
#define MATHLIB_H
#include "stdint.h"
#include <ios>
#include <cmath>
#include <iostream>
#include <string>
#include <unistd.h>
#include <chrono>
#include <fcntl.h>
#include "sys/ioctl.h"
#include "sys/types.h"
#include "linux/i2c-dev.h"
#include "stdint.h"
#include <ctime>                
#include <cstdint>
#include "WPROUTE.h"
//#include "SDL.h"

inline constexpr float PI = 3.14159f;


//READ ME: num of WP vals set in main?
//14 for rn tho

union BFABSUNION {int i; float f;};

float bfabs(float val)
{
  BFABSUNION bfu;
  bfu.f = val;
  bfu.i &= 0x7FFFFFFF;
  return bfu.f;
}

float floatPositionReturn(const std::string msg, int* logPos, int num);

float floatPositionReturn(const std::string msg, int* logPos, int num, int highStart);

int intPositionReturn(const std::string msg, int* logPos, int num);

char charPositionReturn(const std::string msg, int* logPos, int num);

inline
float toFeet(float meters)
{
  return meters * 3.28084;
}

int round(float value);

inline
float latToFeet(float lat)
{
  return lat * 1014.688888;
}

inline
float longToFeet(float Long, float lat)
{
  return (fabs(Long * cos(lat) * 365288.0));
}

inline
int16_t combine(uint8_t* buf, int start) //for converting 2s complement to int16_t
{
  return (~(uint16_t)(buf[start] << 8) | buf[start + 1]) -1;
}

inline
uint16_t radianToFeet(float rad);

inline
float feetToLat(float feet)
{
  return feet / 1014.688888;
}

inline
float feetToLong(float feet, float lat)
{
  return (fabs(feet / (cos(lat) / 365288.0)));
}

float circleDiv(float circleDivft, float radiusft)
{
  return asin(circleDivft/radiusft) * 2.0f;
}

// Define missing types
struct pt3D 
{
    float x, y, z;
};

struct angle
{
float pitch;
float roll;
};

struct flapPos
{
float fL;
float fR;

flapPos& operator+=(const flapPos& other)
{
fL += other.fL;
fR += other.fR;
return *this;
}
void rad(flapPos& myFlapPos);
};

struct FLAP
{
flapPos flap;

/*this func below calculates wether the flap pos is more than +/- 60 deg(in rad) and if the flap is not >60 t = 1, else if  flap >=60 t=0. then either zeros out the flap pos and adj. to */
void flapAdj(FLAP f)
{
  float t1 = fabs(f.flap.fL)/(f.flap.fL + static_cast<float>(!static_cast<int>(fabs(f.flap.fL)+0.9999999f)));
  float t2 = static_cast<float>(!static_cast<int>(fabs(f.flap.fL)/1.047197f));
  f.flap.fR = f.flap.fL = (f.flap.fL * t2) + (1.047197f * ((t2 - 1.0f) * -1.0f * t1 ));
}
};

struct HEMISPHERE
{
char NS;
char EW;
};

struct Time
{
int hrs_;
int min_;
int sec_;
int prev_hrs_;
int prev_min_;
int prev_sec_;
};

inline bool SHUTDOWNERROR = false;

void shutDownErrorCheck()
{
  if(SHUTDOWNERROR)
  {
    std::cout << "SHUTDOWN ERROR" << std::endl;
    //add code to return to main/realio stuff to hand over control to a real user
  }
}


class PLANE;        
template<typename T>
class WPROUTE;
struct PLANEFT;

namespace HMC
{
  class HMC
  {
  public:
  double heading(PLANE* plane);

  private:
  int write(int fd, uint8_t* command);
  int config(int fd);
  int cycle(uint16_t command[3]);
  //look into conv. X/Y/Z to compass dir.
  };
}

class CKALMAN
{
private:
void xPred();
void pPred();
void kGain();
void measureUpdate(PLANE* plane, HMC::HMC* hmc);
void xUpdate();
void updateP();
double magNorthOffset(PLANE* plane);

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

class PLANE
{
public:

//waypoint data for gps
angle WPA;
pt3D loc;

WPROUTE<int> WPXYZ;
//plane data from MPU 6050
angle PA;
float pAngle[3][3];
long long int dt;
long long timePrev;

FLAP ep;
FLAP ap;
FLAP fp;
FLAP rp;

//aspeed 
float speed;
Time time;
int gpsAcc;
HEMISPHERE hemisphere;

int hdop;
int numsat;

float AC[3];

CKALMAN ckalman;
HMC::HMC hmc;
struct PLANEFT
{
float wxft, wyft, wzft;
float pxft, pyft, pzft;
void normalize(PLANE *plane)
{
  wxft = plane->WPXYZ.nextWPpos->x;
  wyft = plane->WPXYZ.nextWPpos->y;
  wzft = plane->WPXYZ.nextWPpos->z;

  pxft = longToFeet(plane->loc.x, plane->loc.z);
  pyft = plane->loc.y;
  pzft = latToFeet(plane->loc.z);
}
};

PLANEFT planeft;

void updateGPS(float lat, float alt, float Long, char hem1, char hem2, float Acc, int hrs, int min, int sec, int Numsat, int Hdop)
{
  loc.x = lat;
  loc.y = alt;
  loc.z = Long;
  hemisphere.NS = hem1;
  hemisphere.EW = hem2;
  gpsAcc = Acc;
  time.prev_hrs_ = time.hrs_;
  time.prev_min_ = time.min_;
  time.prev_sec_ = time.sec_;

  time.hrs_ = hrs;
  time.min_ = min;
  time.sec_ = sec;
  
  numsat = Numsat;
  hdop = Hdop;

  planeft.normalize(this);
}

};
#endif
