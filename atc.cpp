#include "atc.h"

ATC::ATC(int16_t speed, bool engaged): speed_(speed), engaged_(false) {}

void ATC::engaged()
{
  engaged_ = true;
}

void ATC::disengaged()
{
  engaged_ = false;
}

void ATC::setAtcMod(PLANE* plane)
{
  /*atcMod_ : 0.5 = level flight, 1.0 full throttle, 0.0 no throttle*/
  double angleMod = ((double)plane->pAngle[2][0]+(double)plane->pAngle[2][2])/2.0f;
  atcMod_ = sqrt(fabs(0.57735026918075689*tan(angleMod)))/2.0f;
  float posOrNeg = static_cast<float>((!!static_cast<int>(atcMod_+1.0f)<<2)-2);
  atcMod_*=posOrNeg;
}

void ATC::targetSpeed(float value)
{
  /*atcMod_ : 0.5 = level flight, 1.0 full throttle, 0.0 no throttle*/
  speed_ = static_cast<int16_t>(800.0f+(value*800.0f));
}

void ATC::update(PLANE* plane)
{
  if(engaged_)
  {
    setAtcMod(plane);
    targetSpeed(atcMod_);
  }
}
