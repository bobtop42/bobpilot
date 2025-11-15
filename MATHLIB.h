#ifndef MATHLIB_H
#define MATHLIB_H

#include <cmath>
#include <string>
#include "stdint.h"
#include <ios>
#include <cmath>
#include "plane.h"
#include "SDL.h"

float PI = 3.14159f;

//READ ME: num of WP vals set in main?
//14 for rn tho

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


struct pt3D 
{
    float x, y, z;
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

  ap.aileron.fR = ap.aileron.fL = fL;
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

void updateTime(int hrs, int min, int sec, PLANE* plane)
{
  prev_hrs_ = hrs_;
  prev_min_ = min_;
  prev_sec_ = sec_;

  hrs_ = hrs;
  min_ = min;
  sec_ = sec;
}
};

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

float floatPositionReturn(const std::string msg, int* logPos, int num);

float floatPositionReturn(const std::string msg, int* logPos, int num, int highStart);

int intPositionReturn(const std::string msg, int* logPos, int num);

char charPositionReturn(const std::string msg, int* logPos, int num);

inline
float toFeet(float meters)
{
  return meters * 3.28084;
}

inline
uint8_t hexReturn(uint8_t hex)
{
  //return (std::hex(std::showbase(hex)));
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
#endif
