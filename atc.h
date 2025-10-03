#ifndef ATC_H
#define ATC_H
#include "pid.h"
#include "MATHLIB.h"
#include "plane.h"

class ATC
{
public:
int8_t speedAdj;
/*
if plane is at target speed speedAdj -> 0x00
if plane is going slower than target speed speedAdj -> 0x01
if plane is going faster than target speed speedAdj -> 0xFF
*/
uint16_t speed;

void targetSpeed(float value, PLANE* plane);// have target speed update uint16_t speed and maybe plane speed too
void targetSpeed(PLANE* plane); //same as targetSpeed above
void engaged();
void disengaged();
float update(PLANE* plane);
void setUp();
void adjustThrottle(float value, PLANE* plane);

ATC();

private:
bool engaged_;

};

#endif
