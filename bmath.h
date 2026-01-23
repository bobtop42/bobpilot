#pragma once
#include "arm_neon.h"

float bsin(float x); //done
float bcos(float x); //done
float btan(float x);

float bacos(float x);
float basin(float x);
float batan2(float y, float x); //done

float qsqrt(float number); //done
float bfabs(float x); //done
float bfmod(float value, float mod);

union foriunion {float f; int i;};
