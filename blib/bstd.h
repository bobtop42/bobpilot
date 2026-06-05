// Copyright 2026 bobtop42
//
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include <cstdint>
#include "bbitwise.h"

namespace bstd
{
  int32_t stoi(char *chars, int32_t *index);

  template<typename T>
  bool is_same(T a, T b){return true;};

  template<typename T, typename U>
  bool is_same(T a, U b){return false;};

  template<typename T>
  int32_t bit_len(T type);
};
