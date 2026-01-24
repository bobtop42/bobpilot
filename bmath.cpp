#include "bmath.h"

float bsin(float x)
{
  float x2, x3, x5, x7, x9;
  x2 = x*x; x3 = x2*x; x5 = x3*x2; x7 = x5*x2; x9 = x7*x2;

  float32x4_t vx = {x3, x5, x7, x9};
  float32x4_t vdiv = {6.0f, 120.0f, 5040.0f, 362880.0f};

  vx = vdivq_f32(vx, vdiv);

  x -= vgetq_lane_f32(vx, 0);
  x += vgetq_lane_f32(vx, 1);
  x -= vgetq_lane_f32(vx, 2);
  x += vgetq_lane_f32(vx, 3);

  return x;
}

float bcos(float x)
{
  float x2, x4,x6, x8;
  x2 = x*x; x4 = x2*x2; x6 = x4*x2; x8 = x6*x2;

  float32x4_t vx = {x2, x4, x6, x8};
  float32x4_t vdiv = {2.0f, 24.0f, 720.0f, 40320.0f};

  vx = vdivq_f32(vx, vdiv);

  x -= vgetq_lane_f32(vx, 0);
  x += vgetq_lane_f32(vx, 1);
  x -= vgetq_lane_f32(vx, 2);
  x += vgetq_lane_f32(vx, 3);

  return x;
}

float batan2(float y, float x)
{
  foriunion zt1;
  zt1.f = bfabs(x) - bfabs(y);
  zt1.i = (!!(zt1.i & 0x80000000)) * 0x3F800000; //if x>=y zt1 = 0.0f else if x<y zt1 = 1.0f

  foriunion top; foriunion bottom; foriunion temp;
  top.f = bfabs(x) * zt1.f;
  zt1.i ^= 0x3F800000; //zt1 ^= 1.0f
  temp.f = bfabs(y) * zt1.f;
  top.i |= temp.i;

  //if x>=y zt1 = 1.0f else if x<y zt1 = 0.0f
  bottom.f = bfabs(x) * zt1.f;
  zt1.i ^= 0x3F800000; //zt1 ^= 1.0f
  temp.f = bfabs(y) * zt1.f;
  bottom.i |= temp.i;

  temp.f = bfabs(y);
  temp.i = (temp.i & 0x80000000) | 0x3F800000;

  /*zero div check. if bottom=0.0f, top also ==0.0f, so add 1.0f to bottom to prevent zero-div error, and still the result we need after the div*/
  bottom.i = ((!!(bottom.i & 0xFFFFFFFF)) * bottom.i) | ((!(bottom.i & 0xFFFFFFFF)) * 0x3F800000);

  float z = top.f / bottom.f;

  top.f = x; top.i&=0x80000000; //top = sign(x)
  top.i|=0x3F800000;

  float t = z/(1.0f + qsqrt(1.0f + z*z));

  float t2, t3, t5;
  t2 = t*t; t3 = t2*t; t5 = t3*t2;

  float32x2_t vt = {t3, t5};
  float32x2_t vdiv = {3.0f, 5.0f};

  vt = vdivq_f32(vt, vdiv);

  float taylor = t;
  taylor -= vget_lane_f32(vt, 0);
  taylor += vget_lane_f32(vt, 1);

  taylor*=2.0f;

  taylor*=top.f;

  bottom.i = 0x3FC90FDB * (!!zt1.i);
  top.i = 0x3F800000 - (0x3F800000 * !!zt1.i);
  bottom.i |= top.i;

  taylor += bottom.f;
  taylor *= temp.f;

  return taylor;
}

float bsqrt(float number) /*Quake III Arena inverse square root code*/
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;
  
  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

float bfabs(float val)
{
  foriunion bfu;
  bfu.f = val;
  bfu.i &= 0x7FFFFFFF;
  return bfu.f;
}
