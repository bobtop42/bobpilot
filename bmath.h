#pragma once
#include "arm_neon.h"
#include "stdint.h"
#include <cstring>
#define _e_ 2.7182818284590452353602874713527f

float bsin(float x); //done accurate upto 360 deg
float bcos(float x); //done accurate upto 360 deg
float btan(float x);

float bacos(float x);
float basin(float x);
float batan2(float y, float x); //done accurate within ~90deg

float qinverse_qsqrt(float number); // done ~ 1% error margin
float qsqrt(float number); //done ~ 1% error margin

float bfabs(float x); //done accurate always, but no NaN, or inf checks
float bfmod(float value, float mod);

float bln(float x); //done
inline float btrunc(float x){return static_cast<float>(static_cast<int32_t>(x));} //done (duh, inline)

float bpow_no_decimal(float x, float y);//done
float bpow(float x, float y); //done

float bexp_decimal(float x); //done
float bexp_no_decimal(float x); //done
float bexp(float x); //done

float sigmoid(float x); //done
float relu(float x); //done

float sigmoid_derivative(float x); // done
float relu_derivative(float x); //done

union foriunion {float f; int i;};
