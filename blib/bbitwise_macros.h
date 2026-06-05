// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#ifndef BBITWISEMACROS_H
#define BBITWISEMACROS_H

#ifndef signbit64
#define signbit64 0x8000000000000000
#endif

#ifndef signbit32
#define signbit32 0x80000000
#endif

#ifndef signbit16
#define signbit16 0x8000
#endif

#ifndef signbit8
#define signbit8 0x80
#endif

#ifndef data64
#define data64 0x7FFFFFFFFFFFFFFF
#endif

#ifndef data32
#define data32 0x7FFFFFFF
#endif

#ifndef data16
#define data16 0x7FFF
#endif

#ifndef data8
#define data8 0x7F
#endif

#ifndef all64
#define all64 0xFFFFFFFFFFFFFFFF
#endif

#ifndef all32
#define all32 0xFFFFFFFF
#endif

#ifndef all16
#define all16 0xFFFF
#endif

#ifndef all8
#define all8 0xFF
#endif

//reg manipulation
#ifndef fti64
#define fti64 *(int64_t*)&
#endif

#ifndef fti32
#define fti32 *(int32_t*)&
#endif

#ifndef fti16
#define fti16 *(int16_t*)&
#endif

#ifndef fti8
#define fti8 *(int8_t*)&
#endif

#ifndef itf64
#define itf64 *(double*)&
#endif

#ifndef itf32
#define itf32 *(float*)&
#endif

#ifndef itf16
#define itf16 *(float*)&
#endif

#ifndef itf8
#define itf8 *(float*)&
#endif

#endif
