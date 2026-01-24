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
    RollDir = static_cast<int8_t>(value/bfabs(value) + static_cast<float>(!static_cast<int>(bfabs(value) + 0.9999999f)));
    /*
    returns 1 for rolling dir left, 0 for on course for roll, and -1 for rolling dir right.
    code basicaly  does this:
    value += 0.9999999; //makes sure any dec >=1, so when truncates non- zero goes to 1
    static_cast -> int to truncate, then turns all non-zero vals to 0, and 0 -> 1, so when you div, no non-zero errors, then goes back to float, to div then int8_t for RollDir final value
    */
}

void ROLL::targetRoll(PLANE* plane)
{
    float pa = plane->PA.roll; 
    float wp = plane->WPA.roll;

    targetRoll_ = bfabs(wp-pa);

    float t1 = static_cast<float>(!!static_cast<int>(targetRoll_ / 3.14159f));
    targetRoll_ = bfabs(targetRoll_ - (6.28318f * t1));
    
    union pon{int i; float f;};
    pon rollpon; rollpon.f = wp - pa;
    int t3 = static_cast<int>(t1 + !(rollpon.i&0x8000) - 2) & 0x2;
    RollDir = static_cast<int8_t>(t3) - 1*!!rollpon.i;
}

void ROLL::adjustAileron(float value, PLANE* plane)
{
    plane->ap.flap.fR = plane->ap.flap.fL += value * static_cast<float>(RollDir) * -1.0f;
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
