#include "roll.h"

ROLL::ROLL(float targetRoll, float currentRoll, bool engaged):targetRoll_(targetRoll), currentRoll_(currentRoll), engaged_(engaged), RollPID(0.0f, 0.0f, 0.0f) {}

void ROLL::engaged(ROLL& myRoll)
{
    engaged_ = true;
    myRoll.targetRoll();
}

void ROLL::disengaged(ROLL& myRoll)
{
    engaged_ = false;
    myRoll.targetRoll_ = 0.0f;
}

void ROLL::targetRoll()
{
    float pa = 5.0f;  // placeholder for navsys.plane.PA.roll
    float wpa = 1.0f; // placeholder for navsyz.plane.WPA.roll

    if (wpa > pa && pa >= 0.0f)
    {
        // roll right & continue right
        RollDir = Dir::Right;
        targetRoll_ = wpa - pa;
        return;
    }
    else if (wpa < pa && pa <= 0.0f)
    {
        // roll left & continue left
        RollDir = Dir::Left;
        targetRoll_ = pa - wpa;
        return;
    }
    else if (wpa < pa && pa >= 0.0f)
    {
        // roll right & continue left
        RollDir = Dir::Right;
        targetRoll_ = pa - wpa;
        return;
    }
    else if (wpa > pa && pa <= 0.0f)
    {
        // roll left & continue right
        RollDir = Dir::Left;
        targetRoll_ = wpa - pa;
        return;
    }
    else
    {
        // NOTE: add flag raise if on course so
        // user knows and/or something went wrong
        // and defaults to this
        targetRoll_ = 0.0f;
        RollDir = Dir::Straight;
    }
}

void ROLL::update(DESCRETE* myDescrete)
{
    if (engaged_)
    {
        targetRoll();
        myDescrete->calc(myDescrete, targetRoll_);
    }
}
