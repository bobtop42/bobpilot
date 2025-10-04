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
  targetPitch_ = fabs(value);
  PitchDir = static_cast<int8_t>(value/fabs(value) + static_cast<float>(!static_cast<int>(fabs(value))));
  //read target roll for description of this code, its the same logic/value wise, but with pitch instead of roll
  
}

void PITCH::targetPitch(PLANE* plane)
{
  targetPitch_ = fabs(plane->WPA.pitch - plane->PA.pitch);
  PitchDir = (plane->WPA.pitch - plane->PA.pitch)/fabs(plane->WPA.pitch - plane->PA.pitch);
};

void PITCH::adjustElevator(float value, PLANE* plane)
{
  plane->ep.elevator.fL += value * static_cast<float>(PitchDir);
  plane->ep.elevator.fR += value * static_cast<float>(PitchDir);
  plane->ep.elevatorAdj(plane->ep);
}

float PITCH::update(PLANE* plane)
{
  if(engaged_)
  {
    targetPitch(plane);
    float pidAdj = pid.calculate(targetPitch_);
    adjustElevator(pidAdj, plane);
  }
}
