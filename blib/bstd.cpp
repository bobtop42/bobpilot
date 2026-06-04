#include "bstd.h"

int32_t bstd::stoi(char *chars, int32_t *index)
{
  int32_t num = 0, flags = 0, ci = 0, ct = 0, ts = 0, t2 = 0, tc = 0, isNeg = 0;
  while(true)
    {
      ci = (int32_t) chars[*index]; ct = ci - ' '; tc = t2 = iln32(ct);
      flags |= t2<<22;

      ct = ci - '-'; ts = t2 = iln32(ct); isNeg |= ts & ((flags & 0x16) >> 4);
      ct = ci - '+'; ts |= iln32(ct);
      flags |= ((flags & ts) << 1) | (ts << 3) | (ts << 4); ts |= tc;

      ct = ci - '0'; tc = ct >> 31; ct = ci - '9'; t2 &= (ct >>31) ^ 1;
      ct += (num << 3) | (num << 1);
      num = num ^ ((num ^ ct) & (-tc));
      ts |= tc; flags |= (ts ^ 1) << 1;
      
      if((flags & 0x2) == 2){break;}
      index++;
    }
  ct = -num; num ^= ((num ^ ct) & (-isNeg));
  return num;
}
