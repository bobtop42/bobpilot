// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#ifndef ROLL_H
#define ROLL_H
#include "pid.h"
#include "MATHLIB.h"

class ROLL
{
public:
int32_t RollDir;
/*
if plane is straight RollDir -> 0x00
if plane is rolling left RollDir -> 1
if plane is rolling right RollDir -> -1
*/

PID pid{0.1f, 0.1f, 0.1f};

ROLL(float targetRoll, float currentRoll, bool engaged);

void targetRoll(float value, PLANE* plane);
void targetRoll(PLANE* plane);
void engaged();
void disengaged();
void update(PLANE* plane);
void setUp();
void adjustAileron(float value, PLANE* plane);

float targetRoll_;

private:
float currentRoll_;
bool engaged_;

};

#endif
