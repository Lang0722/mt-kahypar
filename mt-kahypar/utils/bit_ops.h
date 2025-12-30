/*******************************************************************************
 * MIT License
 *
 * This file is part of Mt-KaHyPar.
 *
 * Copyright (C) 2019 Lars Gottesbüren <lars.gottesbueren@kit.edu>
 * Copyright (C) 2019 Tobias Heuer <tobias.heuer@kit.edu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#pragma once

#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace mt_kahypar::utils {

inline int popcount_64(const uint64_t x) {
#ifdef _MSC_VER
  return static_cast<int>(__popcnt64(x));
#else
  return __builtin_popcountll(x);
#endif
}

inline int lowest_set_bit_64(const uint64_t x) {
#ifdef _MSC_VER
  unsigned long index;
  if (_BitScanForward64(&index, x)) {
    return static_cast<int>(index);
  }
  return 64;  // Undefined behavior for x == 0, but return consistent value
#else
  return __builtin_ctzll(x);
#endif
}

constexpr int log2(const int x) {
    return x <= 1 ? 0 : 1 + log2(x >> 1);
}

// Constexpr check if a value is a power of 2 (for use in static_assert)
constexpr bool is_power_of_2(const uint64_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

}  // namespace

