#include "atc.h"

ATC::ATC(): engaged_(false) {}

void ATC::engaged()
{
  engaged_ = true;
}

void ATC::disengaged()
{
  engaged_ = false;
}

void ATC::targetSpeed(float value, PLANE* plane)
{
  targetSpeed_ = value; // work on this later
  speedAdj = 0xff;// fix later
  
}

void ATC::targetSpeed(PLANE* plane)
{
  
}

void ATC::update(PLANE* plane)
{
  if(engaged_)
  {
    targetSpeed(plane);
    float pidAdj = pid.calculate(targetSpeed_);
    //finish up
  }
}

