#ifndef REAL_H
#define REAL_H
#include "atc.h"
#include "ckalman.h"
#include "gps.h"
#include "hmc.h"
#include "hold.h"
#include "i2c.h"
#include "kalman.h"
#include "logger.h"
#include "M2H.h"
#include "MATHLIB.h"
#include "pca9685.h"
#include "pid.h"
#include "pitch.h"
#include "plane.h"
#include "roll.h"

class REAL
{
private:
std::string filename = "logTemp.txt";

public:
NAVSYS flightComputer;
HOLD holdComputer;
PLANE plane;

PITCH pitch{0.0f, 0.0f, false};
ROLL roll{0.0f, 0.0f, false};
//ATC atc{0.0f, 0.0f, false};

REAL();

I2C mpu;
GPS gps;
PCA servos;
M2H motors;

LOGGER logger{filename};
KALMAN kalman;

void printStatus();
void setUp();
void update();
void loop();

void targetAll();

void engageAll();
void disengageAll();
void engaged();
void disengaged();

bool isRunning;
bool setUp_ = false;
bool engaged_ = false;

private:
int setUpCounter = 0;
};

#endif
