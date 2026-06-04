// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#ifndef NAVSYS_H
#define NAVSYS_H
#include <cmath>
#include "MATHLIB.h"
#include "WPROUTE.h"
#include "blib/bmath.h"
//NOTE: add npc counter. Burrently only a hardcoded int
#ifdef signbit32
#define signbit32 0x80000000
#endif

class NAVSYS
{
public:

int npc = 0;

WPROUTE<int> WPXYZ;
void setUpRoute();

void wayPointAngleFinder(PLANE* plane);
void planeAngleFinder(CKALMAN* ckalman, PLANE* plane, HMC::HMC* hmc); //fill in with kalman filter stuff later
void updateNpc(PLANE* plane);

void errorCalc(PLANE* plane);

NAVSYS();

uint8_t routeCompleted;

private:
int routeLen;
//float WPXYZ[14][3] = {{40.53081, 0.00f, -111.86214}, {40.53028, 0.00f, -111.86161}};

void updateEP(PLANE& plane, float value);
void updateAP(PLANE& plane, float value);

float px, py, pz;
float wx, wy, wz;

};

union doriunion {double d; int64_t i;};

#endif
