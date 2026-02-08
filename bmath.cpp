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

  foriunion bx; foriunion by;
  bx.f = bfabs(x); by.f = bfabs(y);

  zt1.f = bx.f - by.f;
  zt1.i = (!!(zt1.i & 0x80000000)) * 0x3F800000; //if x>=y zt1 = 0.0f else if x<y zt1 = 1.0f

  foriunion top; foriunion bottom; foriunion temp;

  top.f = bx.f * zt1.f;
  zt1.i ^= 0x3F800000; //zt1 ^= 1.0f
  temp.f = by.f * zt1.f;
  top.i |= temp.i;

  //if x>=y zt1 = 1.0f else if x<y zt1 = 0.0f
  bottom.f = bx.f * zt1.f;
  zt1.i ^= 0x3F800000; //zt1 ^= 1.0f
  temp.f = by.f * zt1.f;
  bottom.i |= temp.i;

  temp.f = by.f;
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
  float32x2_t vmul = {0.3333334f, 0.2f};

  vt = vmul_f32(vt, vmul);

  t -= vget_lane_f32(vt, 0);
  t += vget_lane_f32(vt, 1);

  t*=2.0f;

  t *= top.f;

  bottom.i = 0x3FC90FDB * (!!zt1.i);
  top.i = 0x3F800000 - (0x3F800000 * !!zt1.i);
  bottom.i |= top.i;

  t += bottom.f;
  t *= temp.f;

  return t;
}

float qsqrt(float number) /*Quake III Arena inverse square root code*/
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

  return 1.0f/y;
}

float qinverse_bsqrt(float number) /*Quake III Arena inverse square root code*/
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

float bln(float x)
{
    // normalize x = m * 2^e
    uint32_t bits;
    memcpy(&bits, &x, sizeof(bits));

    int e = ((bits >> 23) & 0xFF) - 127;          // extract exponent
    bits = (bits & 0x7FFFFF) | (127 << 23);      // force exponent to 127 -> m in [1,2)
    float m;
    memcpy(&m, &bits, sizeof(m));

    // atanh transform
    float z = (m - 1.0f) / (m + 1.0f);

    //polynomial
    float z2 = z * z;
    float z3 = z2 * z;
    float z5 = z3 * z2;
    float z7 = z5 * z2;
    float z9 = z7 * z2;

    float ln_m = 2.0f * (z
        + z3 * 0.3333333f
        + z5 * 0.2000000f
        + z7 * 0.1428571f
        + z9 * 0.1111111f);

    // add exponent contribution
    return ln_m + e * 0.6931471805599453f;
}

float bpow_no_decimal(float x, float y)
{
    int i = static_cast<int>(y);
    while(i>0)
    {
        x *= x;
        i--;
    }
    return x;
}

float bexp_decimal(float x) //e^x only for n in range [0,1]
{
  return 1.0f +
         x * (1.0f + 
         x * (0.49999994f + 
         x * (0.16666572f + 
         x * (0.04165735f + 
         x * (0.00830136f + 
         x * (0.0138637f))))));
}

float bexp_integer(float x) //e^x only for Integers
{
  float e = _e_;
  int m = static_cast<int>(bfabs(x)); int i = 0; //sets up loop vals;
  while(i<m){e *= _e_; i++;} //mul e by e m times
  float inverse_check[2];
  i = !(static_cast<int>(x)&0x80000000); //if x is neg, i = 1, else i = 0
  inverse_check[i] = 1.0f; //if x is neg, 1/e^x, else e^x/1
  inverse_check[!i] = e;
  e = inverse_check[0] / inverse_check[1];
  return e;
}

float bexp(float x) // e^x
{
  float integer = static_cast<int>(x);
  float decimal = x - integer;
  float number = bexp_integer(integer) * bexp_decimal(decimal);
  return number;
}

float bpow(float x, float y) /*x^y = 2^k * e ^ y(ln m + e ln 2) - k ln 2 */
{
    /*
    x^y = 2^k * e ^ y(ln m + e ln 2) - k ln 2 

    where:
      x = m * e^2
      k = trunc (y (ln m + e ln 2) * (1/ ln 2))
    */
    uint32_t bits;
    memcpy(&bits, &x, sizeof(bits));

    int e = ((bits >> 23) & 0xFF) - 127;          // extract exponent
    bits = (bits & 0x7FFFFF) | (127 << 23);      // force exponent to 127 -> m in [1,2)
    float m;
    memcpy(&m, &bits, sizeof(m));

    float YlnX = y * (bln(m) + 3.411429f); /* y ln x*/
    float k = btrunc(YlnX * 1.442695f); /*(y ln x) / ln 2*/
    float r = YlnX - k * 0.69314718f; /*XYmain final calc for now*/

    /*2 ^ trunc((y ln x) / ln 2)*/
    uint32_t exp_bits = ((int)k + 127) << 23;
    float pow2k;
    memcpy(&pow2k, &exp_bits, sizeof(pow2k));

    float expR = bexp(r);

    return pow2k * expR;
}

float sigmoid(float x)
{
  return 1.0f / (1.0f + bexp(-x));
}

float relu(float x)
{
  foriunion x_peicewise; x_peicewise.f = x;
  foriunion return_val;
  return_val.i = ((!(x_peicewise.i & 0x80000000)) * x_peicewise.i) | (!!(x_peicewise.i & 0x80000000) * 0x3F800000);
  return return_val.f;
}

float sigmoid_derivative(float x)
{
  float sigmoid_x = sigmoid(x);
  return sigmoid_x * (1.0f - sigmoid_x);
}

float relu_derivative(float x) //relu derivative, but undefined (x = 0) returns 0.0f
{
  foriunion x_peicewise; x_peicewise.f = x;
  foriunion return_val;
  return_val.i = !!(x_peicewise.i & 0x7FFFFFFF) * 0x3F800000;
  return_val.i ^= (!!(x_peicewise.i & 0x80000000)) * return_val.i;
  return return_val.f;
}
