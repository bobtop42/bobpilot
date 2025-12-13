#include <cmath>
#include "MATHLIB.h"
#include "WPROUTE.h"
#ifndef NAVSYS_H
#define NAVSYS_H

//NOTE: add npc counter. Burrently only a hardcoded int

class NAVSYS
{
public:

int npc = 0;

const float PI = 3.14159f;

WPROUTE<int> WPXYZ;

void setUpRoute();

void wayPointAngleFinder(PLANE* plane);
void planeAngleFinder(CKALMAN* ckalman, PLANE* plane, HMC::HMC* hmc); //fill in with kalman filter stuff later
void updateNpc(PLANE* plane);

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

#endif
