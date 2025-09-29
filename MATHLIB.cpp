#include "MATHLIB.h"
#include <string>
#include <cmath>


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
  if(temp > 0.05f)
    value+=0.1f;
  temp= fmod(value,1.0f);
  if(temp >= 0.5f)
    return (static_cast<int>(value+1.0f));
  else
    return (static_cast<int>(value));
}

uint16_t radianToFeet(float rad)
{
  rad*= 651.8892f;
  rad+=2047.0f;
  rad = fmod(rad, 1.0f);
  return (uint16_t)(rad);
}
