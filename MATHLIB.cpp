#include "MATHLIB.h"


void flapPos::rad(flapPos& myFlapPos)
{
  myFlapPos.fL = (myFlapPos.fL / 360.0) * 3.14159;
  myFlapPos.fR = (myFlapPos.fR / 360.0) * 3.14159;
}

float positionReturn(int high, int low)
{
  float lowTemp = static_cast<float>(low);
  while(lowTemp > 1.0f)
    {
      if(lowTemp >=1000.0f)
        lowTemp/=10000.0f;
      if(lowTemp >= 100.0f)
        lowTemp/=100.0f;
      if(lowTemp >= 10.0f)
        lowTemp/=10.0f;
      if(lowTemp >= 1.0f)
        lowTemp/=1.0f;
    }
  return (static_cast<float>(high) + lowTemp);
}

float floatPositionReturn(const std::string msg, int* logPos, int num)
{
  int high = std::stoi(msg.substr(logPos[num]+1, 1));
  int low = std::stoi(msg.substr(logPos[num+1], (logPos[num+2]-logPos[num+1])));
  
  
  float combined = positionReturn(high, low);
  return combined;
}

float floatPositionReturn2(const std::string msg, int* logPos, int num, int highStart)
{
  int high = std::stoi(msg.substr(logPos[num]+1+highStart, 1));
  int low = std::stoi(msg.substr(logPos[num+1], (logPos[num+2]-logPos[num+1])));
  float combined = positionReturn(high, low);
  return combined;
}

int intPositionReturn(const std::string msg, int* logPos, int num)
{
  int value = std::stoi(msg.substr(logPos[num]+1, (logPos[num+1]-logPos[num])));
  return value;
}

char charPositionReturn(const std::string msg, int* logPos, int num)
{
  char ch = std::stoi(msg.substr(logPos[num]+1, 1));
  return ch;
};

int round(float value)//check the if statements on this
{
  value += 0.01f;
  float temp = fmod(value, 0.1f);
  value += (static_cast<float>(!static_cast<int>(temp+0.9999999f))*0.1f);
  temp= fmod(value, 1.0f);
  return static_cast<int>(value+(static_cast<float>(static_cast<int>(temp+0.5f))*1.0f));
}

uint16_t radianToFeet(float rad)
{
  rad*= 651.8892f;
  rad+=2047.0f;
  rad = fmod(rad, 1.0f);
  return (uint16_t)(rad);
}

void FLAP::flapAdj(FLAP f)
{
  float t1 = bfabs(f.flap.fL)/(f.flap.fL + static_cast<float>(!static_cast<int>(bfabs(f.flap.fL)+0.9999999f)));
  float t2 = static_cast<float>(!static_cast<int>(bfabs(f.flap.fL)/1.047197f));
  f.flap.fR = f.flap.fL = (f.flap.fL * t2) + (1.047197f * ((t2 - 1.0f) * -1.0f * t1 ));
}

void PLANE::updateGPS(float lat, float alt, float Long, char hem1, char hem2, float Acc, int hrs, int min, int sec, int Numsat, int Hdop)
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
