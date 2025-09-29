#ifndef HOLD_H
#define HOLD_H
#include <cstdint>
#include "MATHLIB.h"
#include "realio.h"
#include "pitch.h"
#include "roll.h"
#include "plane.h"

class HOLD
{
public:
void prePosCalc(int radiusft, float alt, float lat, float lng, int8_t clockWise, PLANE* plane);
void targetHeading(PLANE* plane, PITCH* pitch, ROLL* roll);
void holdAlt(float altft, PLANE* plane);
void holdHeading(float dir, PLANE* pkane);
void heading(float dir , PLANE* plane, ROLL* roll);
void alt(float alt, PLANE* plane, PITCH* pitch);

float roll;
float pitch;

private:
float px; float tx;
float py; float ty;
float pz; float tz;

float Wpitch; float Ppitch;
float Wroll; float Proll;
};

#endif
