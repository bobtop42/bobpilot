#ifndef MATHLIB_H
#define MATHLIB_H
#include "stdint.h"
#include <ios>
#include <cmath>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "sys/ioctl.h"
#include "sys/types.h"
#include "linux/i2c-dev.h"
#include "stdint.h"
#include <ctime>                
#include <cstdint>
//#include "SDL.h"

float PI = 3.14159f;

//READ ME: num of WP vals set in main?
//14 for rn tho

/*
struct PlaneObj
{
  float planePos[6][3]
  {
    {7.0f,1.0f,4.0f}, //bottom fold, point 0, connects with points: 2,3,4
    {7.0f,2.0f,1.0f}, //left wing tip, point 1, connects with points: 2,3
    {7.0f,2.0f,3.5f}, //left top fold, point 2, connects with points: 0,1,3
    {1.0f,2.0f,4.0f}, //nose, point 3, connects with points: 1,2,4,5
    {7.0f,2.0f,4.5f}, //right top fold, point 4, connects with points: 0,3,5
    {7.0f,2.0f,7.0f} //right wing tip, point 5, connects with points: 3,4
  };
  int planeScreenPos[6][2];

  void undatePlaneScreenPos(int posNorMul, int x, int y)
  {
    
  }

  void drawPlane(SDL_Renderer* renderer)
  {
      SDL_Set_Render_Draw_Color(renderer,89,59,255,1);
    SDL_Render_Draw_Line(renderer, )
  }
};
*/

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



/*
NOTE: 
elevatorAdj and aileronAdj are now branchless. i got really bored. a lot of stuff is now branchless b/c i got bored.
*/
struct EP 
{
flapPos elevator;
void elevatorAdj(EP ep)
{
  {
    float fL = ep.elevator.fL;
    float t = static_cast<float>(static_cast<int>(fabs(fL)/1.047197f));
    fL = (fL * (1.0f - t)) + (1.047197f * t * (fL/fabs(fL) + (static_cast<float>(!static_cast<int>(fabs(fL) + 0.9999999f)))));

    ep.elevator.fL = fL;
    ep.elevator.fR = fL;

    /*
    this calculate wether the abs of fL is more than 60deg (in rads tho), if so t=1, else t=0. 
    t either zeros out if its less that 60, keeping the elevator pos or make fL eq 60deg if it reads more than 60deg, by zeroing out the elevator pos, and setting to 60deg (either +/-). 
    */
  }
}
};

struct AP
{
  flapPos aileron;
  void aileronAdj(AP ap)
  {
    //read elevatorAdj for note on this code
    float fL = ap.aileron.fL;
    float t = static_cast<float>(static_cast<int>(fabs(fL)/1.047197f));
    fL = (fL * (1.0f - t)) + (1.047197f * t * (fL/fabs(fL) + (static_cast<float>(!static_cast<int>(fabs(fL) + 0.9999999f)))));
    ap.aileron.fL = fL;
    ap.aileron.fR = fL * -1.0f;
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
bool SHUTDOWNERROR = false;

void shutDownErrorCheck()
{
  if(SHUTDOWNERROR)
  {
    std::cout << "SHUTDOWN ERROR" << std::endl;
    //add code to return to main/realio stuff to hand over control to a real user
  }
}


class PLANE;        

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
  time.prev_hrs_ = time.hrs_;
  time.prev_min_ = time.min_;
  time.prev_sec_ = time.sec_;

  time.hrs_ = hrs;
  time.min_ = min;
  time.sec_ = sec;
}

};
#endif
