#ifndef PITCH_H
#define PITCH_H
#include "pid.h"
#include "MATHLIB.h"
#include "plane.h"

class PITCH
{
public:
int8_t PitchDir;
/*
if plane is straight PitchDir -> 0x00
if plane is pitching up PitchDir -> 0xFF
if plane is pitching down PitchDir -> 0x01
*/

PID pid{0.1f, 0.1f, 0.1f};

PITCH(float targetPitch, float currentPitch, bool engaged);

void targetPitch(float value, PLANE* plane);
void targetPitch(PLANE* plane);
void engaged();
void disengaged();
float update(PLANE* plane);
void setUp();
void adjustElevator(float value, PLANE* plane);

float targetPitch_;

private:
float currentPitch_;
bool engaged_;

};
  
#endif 
