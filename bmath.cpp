#include "bmath.h"

extern "C"
{
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

  int babs(int n)
  {
      int32_t mul = ((!(n & signbit32))<<1) - 0x1;
      return n * mul;
  }

float bfloor(float n)
{
    foriunion n1; n1.f = n;
    foriunion nt; nt.f = n; 
    float n2;
    nt.i = (!!(nt.i & signbit32)) * 0x3F800000 * (!!(nt.i & 0x7FFFFFFF));
    n1.f = n1.f - static_cast<float>(static_cast<int32_t>(n1.f));
    nt.i *= !!n1.i;
    n -= nt.f;
    return static_cast<float>(static_cast<int32_t>(n));
}

float bceil(float n)
{
    foriunion n1; n1.f = n;
    foriunion nt; nt.f = n; 
    float n2;
    nt.i = (!(nt.i & signbit32)) * 0x3F800000 * (!!(nt.i & 0x7FFFFFFF));
    n1.f = n1.f - static_cast<float>(static_cast<int32_t>(n1.f));
    nt.i *= !!n1.i;
    n += nt.f;
    return static_cast<float>(static_cast<int32_t>(n));
}

float bround(float x)
{
    foriunion n; n.f = x;
    int32_t sign = n.i & signbit32;

    n.i &= 0x7FFFFFFF;

    n.f += 0.5;
    n.i = static_cast<int32_t>(n.f);
    n.i |= sign;
    return n.f;
}

float bln(float x)
{
  foriunion xn; xn.f = x;
  int32_t k = ((xn.i >> 23) & 0xFF) - 127;
  xn.i = (xn.i & 0x007FFFFF) | 0x3F800000;
  float m = xn.f;
  xn.f = m - SQRT2; 
  xn.i = !(xn.i & signbit32) | (!(xn.i & 0x7FFFFFFF));  
  k += xn.i;
  xn.i = ((0x3F800000 ^ (xn.i << 23))); 
  m *= xn.f; 

      // poly coeffs
  const float c0 = -2.72087514853282908e+00f;
  const float c1 =  7.99381310026068803e+00f;
  const float c2 = -1.38728351690689511e+01f;
  const float c3 =  1.82053222322546624e+01f;
  const float c4 = -1.66727754381288804e+01f;
  const float c5 =  1.03467182950741474e+01f;
  const float c6 = -4.14977469835951318e+00f;
  const float c7 =  9.71206070689384804e-01f;
  const float c8 = -1.00799216432559263e-01f;
  
  // estrin grouping
  float m2 = m * m;
  float m4 = m2 * m2;
  float m6 = m4 * m2;
  float m8 = m4 * m4;
  
  float g1 = c0 + c1 * m;
  float g2 = c2 + c3 * m;
  float g3 = c4 + c5 * m;
  float g4 = c6 + c7 * m;
  
  g1 = g1 + g2 * m2;
  g1 = g1 + g3 * m4;
  g1 = g1 + g4 * m6 + c8 * m8;
  
  xn.f = k * LN2 + g1;
  k = *(int32_t*)&x; k -= 0x3F800000;
  xn.i *= !!(k & 0x7FFFFFFF);
  k = *(int32_t*)&x; k = ((!(k & 0x7FFFFFFF)) | (!!(k & 0x80000000))) * 0xFF800000;
  xn.i = (xn.i * (!k)) | k;
  return xn.f;
  }

float bldexp(float x, int e)
{
    int twopow = 0x1 << babs(e);
    int an = !(e & signbit32);
    float flip[2];
    flip[!an] = static_cast<float>(twopow);
    flip[an] = 1.0f;
    return (flip[0]/flip[1]) * x;
}

float bexp(float x)
{
  #define EXP_C0 1.00000007548957037e+00f
  #define EXP_C1 1.00000006470314284e+00f
  #define EXP_C2 4.99988691473023439e-01f
  #define EXP_C3 1.66663256445656882e-01f
  #define EXP_C4 4.19175264833232322e-02f
  #define EXP_C5 8.38111203742408079e-03f

  int k = (int)bround(x * INV_LN2);
  float r = x - k * LN2;
  float e = EXP_C5;
  e = e * r + EXP_C4;
  e = e * r + EXP_C3;
  e = e * r + EXP_C2;
  e = e * r + EXP_C1;
  e = e * r + EXP_C0;
  return bldexp(e, k);

  #undef EXP_C0
  #undef EXP_C1
  #undef EXP_C2
  #undef EXP_C3
  #undef EXP_C4
  #undef EXP_C5
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

  /* LOOK INTO MATH BELOW
  foriunion sigmoid_val; sigmoid_val.f = x;
  foriunion sigmoid_temp; sigmoid_temp.i = !(sigmoid_val.i & 0x80000000);
  sigmoid_val.i |= 0x80000000;
  foriunion bexp_x; bexp_x.f = bexp(sigmoid_val.f);
  foriunion top; top.i = (0x3F800000 * sigmoid_temp.i) | (!sigmoid_temp.i * bexp_x.i);
  return top.f / (1.0f + bexp_x.f);
  */
  return 1.0f / (1.0f + bexp(-x));
}

float relu(float x)
{
  foriunion x_peicewise; x_peicewise.f = x;
  foriunion return_val;
  return_val.i = ((!(x_peicewise.i & signbit32)) * x_peicewise.i);
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
  return_val.i = ((!!(x_peicewise.i & 0x7FFFFFFF)) & (!(x_peicewise.i & signbit32))) * 0x3F800000;
  return return_val.f;
}

/*NaN, +/- inf. check func*/
uint8_t validate_number(float n)
{
  /*
  NaN = 0x7FC00000, return 0x1
  +inf = 0x7F800000, return 0x2
  -inf = 0xFF800000, return 0x3
  */
}
}
