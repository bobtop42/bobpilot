#ifndef ROLL_H
#define ROLL_H
#include "pid.h"
#include "MATHLIB.h"
#include "plane.h"

class ROLL
{
public:
int8_t RollDir;
/*
if plane is straight RollDir -> 0x00
if plane is rolling left RollDir -> 0xFF
if plane is rolling right RollDir -> 0x01
*/

PID pid{0.1f, 0.1f, 0.1f};

ROLL(float targetRoll, float currentRoll, bool engaged);

void targetRoll(float value, PLANE* plane);
void targetRoll(PLANE* plane);
void engaged();
void disengaged();
float update(PLANE* plane);
void setUp();
void adjustAileron(float value, PLANE* plane);

float targetRoll_;

private:
float currentRoll_;
bool engaged_;

};

#endif
