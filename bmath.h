#pragma once
#include "arm_neon.h"
#include "stdint.h"
#include <cstring>
#define _e_ 2.7182818284590452353602874713527f
#define LN2_HI 0.6931457519f
#define LN2_LO 1.42860677e-6f
#ifndef signbit32
#define signbit32 0x80000000
#endif

extern "C"
{
float bsin(float x); //done accurate upto 360 deg
float bcos(float x); //done accurate upto 360 deg
float btan(float x);

float bacos(float x);
float basin(float x);
float batan2(float y, float x); //done accurate within ~90deg

float qinverse_qsqrt(float number); // done ~ 1% error margin
float qsqrt(float number); //done ~ 1% error margin

float bfabs(float x); //done accurate always, but no NaN, or inf checks
float babs(int n);

float bfloor(float n);
float bceil(float n);

float bround(float x);
float bfmod(float value, float mod);

float bln(float x); //done
inline float btrunc(float x){return static_cast<float>(static_cast<int32_t>(x));} //done (duh, inline)

float bpow(float x, float y); //done

float bexp(float x); //done
float bldexp(float x, int e);

float sigmoid(float x); //done
float relu(float x); //done

float sigmoid_derivative(float x); // done
float relu_derivative(float x); //done

uint8_t validate_number(float n);

union foriunion {float f; int i;};
union foruiunion {float f; uint32_t ui;};

int32_t offbits(float f1, float f2);
int32_t ulp(float a, float b);
}
