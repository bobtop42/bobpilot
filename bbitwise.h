// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include "bbitwise_macros.h"
#include <cstdint>

#ifndef BBITWISE_H
#define BBITWISE_H
//  MACROS FROM bbitwise_macros.h:
//  +------------+------------------+-----------------------------------+  //
//  | name       | sizes            | purpose                           |  //
//  |------------+------------------+-----------------------------------|  //
//  | signbit    | 64,32,16,8       | reads only sign bit               |  //
//  |------------+------------------+-----------------------------------|  //
//  | data       | 64,32,16,8       | reads only data bits              |  //
//  |------------+------------------+-----------------------------------|  //
//  | fti        | 64,32            | returns reg float data to int     |  //
//  |------------+------------------+-----------------------------------|  //
//  | itf        | 64,32            | returns reg int data to float     |  //
//  +------------+------------------+-----------------------------------+  //
//
//  FUNCTIONS IN bbitwise.h:
//  +------------+------------------+-----------------------------------+  //
//  |name        | sizes            | purpose                           |  //
//  |------------+------------------+-----------------------------------|  //
//  | fln        | 64,32,16,8       | returns logical ! of a input ints |  //
//  |            |                  | register of a floating point val  |  //
//  |            |                  | this func returns 1 for -0 too    |  //
//  |------------+------------------+-----------------------------------|  //
//  | fdln       | 64,32,16,8       | returns logical !! of a input ints|  //
//  |            |                  | register of a floating point val  |  //
//  |            |                  | this func returns 0 for -0 too    |  //
//  |------------+------------------+-----------------------------------|  //
//  | iln        | 64,32,16,8       | returns logical ! of a input ints |  //
//  |            |                  | register of a integer val         |  //
//  |------------+------------------+-----------------------------------|  //
//  | idln       | 64,32,16,8       | returns logical !! of a input ints|  //
//  |            |                  | register of a integer val         |  //
//  |------------+------------------+-----------------------------------|  //
//  | fsign      | 64,32,16,8       | returns sign of a input ints      |  //
//  |            |                  | register of a floating point val  |  //
//  |            |                  | this func returns 1 for -0 too    |  //
//  |------------+------------------+-----------------------------------|  //
//  | isign      | 64,32,16,8       | returns sign of a input ints      |  //
//  |            |                  | register of a integer val         |  //
//  |------------+------------------+-----------------------------------|  //
//  | fzsign     | 64,32,16,8       | returns sign of a input ints      |  //
//  |            |                  | register of a floating point val  |  //
//  |            |                  | this func returns 0 for neg, and  |  //
//  |            |                  | 1 for pos, 0 and -0               |  //
//  |------------+------------------+-----------------------------------|  //
//  | izsign     | 64,32,16,8       | returns sign of a input ints      |  //
//  |            |                  | register of a integer val         |  //
//  |            |                  | this func returns 0 for neg, and  |  //
//  |            |                  | 1 for pos, 0                      |  //
//  +------------+------------------+-----------------------------------+  //

inline int64_t fln64(int64_t n){return (((n | (-n)) >> 62) ^ 1) & 1;}
inline int32_t fln32(int32_t n){return (((n | (-n)) >> 30) ^ 1) & 1;}
inline int16_t fln16(int16_t n){return (((n | (-n)) >> 14) ^ 1) & 1;}
inline int8_t fln8(int8_t n){return (((n| (-n)) >> 6) ^ 1) & 1;}

inline int64_t fdln64(int64_t n){return ((n | (-n)) >> 62) & 1;}
inline int32_t fdln32(int32_t n){return ((n | (-n)) >> 30) & 1;}
inline int16_t fdln16(int16_t n){return ((n | (-n)) >> 14) & 1;}
inline int8_t fdln8(int8_t n){return ((n | (-n)) >> 6) & 1;}

inline int64_t iln64(int64_t n){return (((n | (-n)) >> 63) ^ 1) & 1;}
inline int32_t iln32(int32_t n){return (((n | (-n)) >> 31) ^ 1) & 1;}
inline int16_t iln16(int16_t n){return (((n | (-n)) >> 15) ^ 1) & 1;}
inline int8_t iln8(int8_t n){return (((n| (-n)) >> 7) ^ 1) & 1;}

inline int64_t idln64(int64_t n){return ((n | (-n)) >> 63) & 1;}
inline int32_t idln32(int32_t n){return ((n | (-n)) >> 31) & 1;}
inline int16_t idln16(int16_t n){return ((n | (-n)) >> 15) & 1;}
inline int8_t idln8(int8_t n){return ((n | (-n)) >> 7) & 1;}

inline int64_t fsign64(int64_t n){return (((((n & signbit64) >> 63) ^ 1) | fln64(n)) << 1) - 1;}
inline int32_t fsign32(int32_t n){return (((((n & signbit32) >> 31) ^ 1) | fln32(n)) << 1) - 1;}
inline int16_t fsign16(int16_t n){return (((((n & signbit16) >> 15) ^ 1) | fln16(n)) << 1) - 1;}
inline int8_t fsign8(int8_t n){return (((((n & signbit8) >> 7) ^ 1) | fln8(n)) << 1) - 1;}

inline int64_t fzsign64(int64_t n){return (((n & signbit64) >> 63) ^ 1) | fln64(n);}
inline int32_t fzsign32(int32_t n){return (((n & signbit64) >> 63) ^ 1) | fln64(n);}
inline int16_t fzsign16(int16_t n){return (((n & signbit64) >> 63) ^ 1) | fln64(n);}
inline int8_t fzsign8(int8_t n){return (((n & signbit64) >> 63) ^ 1) | fln64(n);}

inline int64_t isign64(int64_t n){return ((((~n)&signbit64)>>62)-1);}
inline int32_t isign32(int32_t n){return ((((~n)&signbit32)>>30)-1);}
inline int16_t isign16(int16_t n){return ((((~n)&signbit16)>>14)-1);}
inline int8_t isign8(int8_t n){return ((((~n)&signbit8)>>6)-1);}

inline int64_t izsign64(int64_t n){return (((~n)&signbit64)>>63);}
inline int32_t izsign32(int32_t n){return (((~n)&signbit32)>>31);}
inline int16_t izsign16(int16_t n){return (((~n)&signbit16)>>15);}
inline int8_t izsign8(int8_t n){return (((~n)&signbit8)>>7);}

#endif
