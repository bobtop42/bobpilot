#include "pitch.h"

PITCH::PITCH(float targetPitch, float currentPitch, bool engaged): targetPitch_(targetPitch), currentPitch_(currentPitch), engaged_(engaged) {};

void PITCH::engaged()
{
  engaged_ = true;
};

void PITCH::disengaged()
{
  engaged_ = false;
};

void PITCH::targetPitch(float value, PLANE* plane)
{
  targetPitch_ = bfabs(value);
  PitchDir = static_cast<int8_t>(value/bfabs(value) + static_cast<float>(!static_cast<int>(bfabs(value))));
  //read target roll for description of this code, its the same logic/value wise, but with pitch instead of roll
  
}

void PITCH::targetPitch(PLANE* plane)
{
  targetPitch_ = bfabs(plane->WPA.pitch - plane->PA.pitch);
  PitchDir = (plane->WPA.pitch - plane->PA.pitch)/bfabs(plane->WPA.pitch - plane->PA.pitch);
};

void PITCH::adjustElevator(float value, PLANE* plane)
{
  plane->ep.flap.fR = plane->ep.flap.fL += value * static_cast<float>(PitchDir);
  plane->ep.flapAdj(plane->ep);
}

void PITCH::update(PLANE* plane)
{
  if(engaged_)
  {
    targetPitch(plane);
    float pidAdj = pid.calculate(targetPitch_);
    adjustElevator(pidAdj, plane);
  }
}
