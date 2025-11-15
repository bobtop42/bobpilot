#include <cmath>
#include "plane.h"
#include "io.h"
#ifndef NAVSYS_H
#define NAVSYS_H

//NOTE: add npc counter. Burrently only a hardcoded int

class NAVSYS
{
public:
PLANE *navsys = &plane;

int npc = 0;

const float PI = 3.14159f;

void wayPointAngleFinder();
void planeAngleFinder(CKALMAN* ckalman, PLANE* plane, HMC::HMC* hmc); //fill in with kalman filter stuff later
void updateNpc(PLANE* plane);

NAVSYS();

uint8_t routeCompleted;

private:
int routeLen;
float WPXYZ[14][3];

void updateEP(PLANE& plane, float value);
void updateAP(float value);

};

#endif
