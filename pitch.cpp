#include "pitch.h"

PITCH::PITCH(float targetpitch, float currentpitch, bool engaged):targetPitch_(targetpitch), currentPitch_(currentpitch), engaged_(engaged) {}

void PITCH::engaged()
{
    engaged_ = true;
}

void PITCH::disengaged()
{
    engaged_ = false;
}

void PITCH::targetPitch(float value, PLANE* plane)
{
    targetPitch_ = bfabs(value);
    foriunion pitchPoN; pitchPoN.f = value;
    PitchDir = (((!(pitchPoN.i & 0x80000000))<<1)-1) * (!!(pitchPoN.i & 0x7FFFFFFF));
    /*
    returns -1 for pitching dir left, 0 for on course for pitch, and 1 for pitching dir right.
    code basicaly  does this:
    value += 0.9999999; //makes sure any dec >=1, so when truncates non- zero goes to 1
    static_cast -> int to truncate, then turns all non-zero vals to 0, and 0 -> 1, so when you div, no non-zero errors, then goes back to float, to div then int8_t for pitchDir final value
    */
}

void PITCH::targetPitch(PLANE* plane)
{
    float pa = plane->PA.pitch;
    float wp = plane->WPA.pitch;

    foriunion pitchPoN; pitchPoN.f = wp - pa;
    foriunion isbig; isbig.f = bfabs(pitchPoN.f) - 3.141592f;
    isbig.i = !(isbig.i & 0x80000000);
    PitchDir = (((!((!(pitchPoN.i & 0x80000000))^(!isbig.i)))<<1)-1)*(!!(pitchPoN.i & 0x7FFFFFFF));
    isbig.i = (isbig.i * 0x40C90FDB) | (PitchDir & 0x80000000);
    targetPitch_ = bfabs(isbig.f + pitchPoN.f);
}

void PITCH::adjustElevator(float value, PLANE* plane)
{
    plane->ap.flap.fR = plane->ap.flap.fL += value * static_cast<float>(PitchDir);
    plane->ap.flapAdj(plane->ap);
}

void PITCH::update(PLANE* plane)
{
    if (engaged_)
    {
        targetPitch(plane);
        float pidAdj = pid.calculate(targetPitch_);
        adjustElevator(pidAdj, plane);
    }
}
