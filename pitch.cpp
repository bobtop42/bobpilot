#include "pitch.h"

PITCH::PITCH(float targetPitch, float currentPitch, bool engaged): targetPitch_(targetPitch), currentPitch_(currentPitch), engaged_(engaged), PitchPID(0.0f, 0.0f, 0.0f) {};

void PITCH::engaged(PITCH& myPitch)
{
  engaged_ = true;
  myPitch.targetPitch();
};

void PITCH::disengaged(PITCH& myPitch)
{
  engaged_ = false;
  myPitch.targetPitch_ = 0.0f;
};

void PITCH::targetPitch()
{
  float pa = 5.0f; //placeholder for navsys.plane.PA.pitch
  float wpa = 1.0f; //placeholder for navsyz.plane.WPA.pitch
  if(wpa>pa && pa>=0.0f)
  {
    //pitch up & cont up
    PitchDir = Dir::Up;
    targetPitch_ = wpa - pa;
    return;
  }
  else if(wpa<pa && pa<=0.0f)
  {
    //pitch down & cont down
    PitchDir = Dir::Down;
    targetPitch_ = pa - wpa;
    return;
  }
  else if(wpa<pa && pa>=0.0f)
  {
    //pitch up & cont down
    PitchDir = Dir::Up;
    targetPitch_ = pa - wpa;
    return;
  }
  else if(wpa>pa && pa<=0.0f)
  {
    //pitch down & cont up
    PitchDir = Dir::Down;
    targetPitch_ = wpa - pa;
    return;
  }
  else
  {
    //NOTE: add flag raise if on course so
    //user knows &/or smthong went wrong
    //and defaults to this
    targetPitch_ = 0.0f;
    PitchDir = Dir::Level;
  }
};

void PITCH::update(DESCRETE *myDescrete)
{
  if(engaged_)
  {
    targetPitch();

    myDescrete->calc(myDescrete, targetPitch_);    
  }
}
