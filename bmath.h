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
#ifndef LN2
#define LN2 0.69314718055994530941723212145818f
#endif
#ifndef INV_LN2
#define INV_LN2 1.4426950408889634073599246810019f
#endif
#ifndef SQRT2
#define SQRT2 1.41421356237f
#endif
#ifndef SQRT2PLUS0x1
#define SQRT2PLUS0x1 1.4142137f
#endif

extern "C"
{
float bsin(float x); //todo
float bcos(float x); //todo
float btan(float x); //todo

float bacos(float x); //todo
float basin(float x); //todo
float batan2(float y, float x); //finish

float qinverse_qsqrt(float number); // done ~ 1% error margin
float qsqrt(float number); //done ~ 1% error margin

float bfabs(float x); //done accurate always, but no NaN, or inf checks
int babs(int n); //done accurate always, but no NaN, or inf checks

float bfloor(float n); //done
float bceil(float n); //done

float bround(float x); //done
float bfmod(float value, float mod); //todo

float bln(float x); //done
inline float btrunc(float x){return static_cast<float>(static_cast<int32_t>(x));} //done (duh, inline)

float bpow(float x, float y); //finish

float bexp(float x); //done
float bldexp(float x, int e); //done

float sigmoid(float x); //done?
float relu(float x); //done?

float sigmoid_derivative(float x); //done?
float relu_derivative(float x); //done?

uint8_t validate_number(float n);

union foriunion {float f; int i;};
union foruiunion {float f; uint32_t ui;};

int32_t offbits(float f1, float f2);
int32_t ulp(float a, float b);
}
