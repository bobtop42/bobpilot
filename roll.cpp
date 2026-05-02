#include "roll.h"

ROLL::ROLL(float targetRoll, float currentRoll, bool engaged):targetRoll_(targetRoll), currentRoll_(currentRoll), engaged_(engaged) {}

void ROLL::engaged()
{
    engaged_ = true;
}

void ROLL::disengaged()
{
    engaged_ = false;
}

void ROLL::targetRoll(float value, PLANE* plane)
{
    targetRoll_ = bfabs(value);
    foriunion rollPoN; rollPoN.f = value;
    RollDir = (((!(rollPoN.i & 0x80000000))<<1)-1) * (!!(rollPoN.i & 0x7FFFFFFF));
    /*
    returns -1 for rolling dir left, 0 for on course for roll, and 1 for rolling dir right.
    code basicaly  does this:
    value += 0.9999999; //makes sure any dec >=1, so when truncates non- zero goes to 1
    static_cast -> int to truncate, then turns all non-zero vals to 0, and 0 -> 1, so when you div, no non-zero errors, then goes back to float, to div then int8_t for RollDir final value
    */
}

void ROLL::targetRoll(PLANE* plane)
{
    float pa = plane->PA.roll;
    float wp = plane->WPA.roll;

    foriunion rollPoN; rollPoN.f = wp - pa;
    foriunion isbig; isbig.f = bfabs(rollPoN.f) - 3.141592f;
    isbig.i = !(isbig.i & 0x80000000);
    RollDir = (((!((!(rollPoN.i & 0x80000000))^(!isbig.i)))<<1)-1)*(!!(rollPoN.i & 0x7FFFFFFF));
    isbig.i = (isbig.i * 0x40C90FDB) | (RollDir & 0x80000000);
    targetRoll_ = bfabs(isbig.f + rollPoN.f);
}

void ROLL::adjustAileron(float value, PLANE* plane)
{
    plane->ap.flap.fR = plane->ap.flap.fL += value * static_cast<float>(RollDir);
    plane->ap.flapAdj(plane->ap);
}

void ROLL::update(PLANE* plane)
{
    if (engaged_)
    {
        targetRoll(plane);
        float pidAdj = pid.calculate(targetRoll_);
        adjustAileron(pidAdj, plane);
    }
}
