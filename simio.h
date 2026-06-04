// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#ifndef SIM_H
#define SIM_H
#include "Flight_Controls/atc.h"
#include "drivers/gps.h"
#include "hold.h"
#include "drivers/i2c.h"
#include "kalman.h"
#include "logger.h"
#include "MATHLIB.h"
#include "drivers/pca9685.h"
#include "pid.h"
#include "Flight_Controls/pitch.h"
#include "Flight_Controls/roll.h"
#include "screen.h"

class SIM
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

SIM();

I2C mpu;
GPS gps;
PCA servos;

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
