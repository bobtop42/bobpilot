// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#ifndef ATC_H
#define ATC_H
#include "pid.h"
#include "MATHLIB.h"
#include "bmath.h"

class ATC
{
public:
int8_t atcAdj;
/*
if plane is at target speed speedAdj -> 0x00
if plane is going slower than target speed speedAdj -> 0x01
if plane is going faster than target speed speedAdj -> 0xFF
*/

void targetSpeed(float value);// have target speed update uint16_t speed and maybe plane speed too
void setAtcMod(PLANE* plane); //same as targetSpeed above
void engaged();
void disengaged();

uint16_t returnSpeed();
float returnAtcMod();

void update(PLANE* plane);

ATC(int16_t speed, bool engaged);

int16_t speed_;

bool engaged_;
float atcMod_ = 0.0f;

};

#endif
