#pragma once
#include "arm_neon.h"

float bsin(float x); //done accurate upto 360 deg
float bcos(float x); //done accurate upto 360 deg
float btan(float x);

float bacos(float x);
float basin(float x);
float batan2(float y, float x); //done accurate within ~90deg

float qsqrt(float number); //done ~ 1% error margin
float bfabs(float x); //done accurate always, but no NaN, or inf checks
float bfmod(float value, float mod);

union foriunion {float f; int i;};
