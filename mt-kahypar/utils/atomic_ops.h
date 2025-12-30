/*******************************************************************************
 * MIT License
 *
 * This file is part of Mt-KaHyPar.
 *
 * Copyright (C) 2024 Mt-KaHyPar Contributors
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

/**
 * This header provides portable atomic operations that work on both
 * GCC/Clang (using __atomic_* intrinsics) and MSVC (using _Interlocked* intrinsics).
 *
 * The memory orderings are simplified to match common use cases:
 * - RELAXED: No ordering guarantees (fastest)
 * - ACQUIRE: Prevents reordering of subsequent reads/writes
 * - RELEASE: Prevents reordering of preceding reads/writes
 * - SEQ_CST: Sequential consistency (strongest, default)
 */

#include <cstdint>
#include <type_traits>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace mt_kahypar {

// Memory order constants for portable use
enum class MemoryOrder {
  Relaxed,
  Acquire,
  Release,
  AcqRel,
  SeqCst
};

namespace detail {

#ifdef _MSC_VER

// MSVC implementations using _Interlocked* intrinsics

template<typename T>
inline T atomic_load(const T* ptr, MemoryOrder order) {
  static_assert(sizeof(T) <= 8, "Atomic operations only support types up to 64 bits");
  T value;
  if constexpr (sizeof(T) == 1) {
    value = static_cast<T>(_InterlockedOr8(reinterpret_cast<volatile char*>(const_cast<T*>(ptr)), 0));
  } else if constexpr (sizeof(T) == 2) {
    value = static_cast<T>(_InterlockedOr16(reinterpret_cast<volatile short*>(const_cast<T*>(ptr)), 0));
  } else if constexpr (sizeof(T) == 4) {
    value = static_cast<T>(_InterlockedOr(reinterpret_cast<volatile long*>(const_cast<T*>(ptr)), 0));
  } else {
    value = static_cast<T>(_InterlockedOr64(reinterpret_cast<volatile long long*>(const_cast<T*>(ptr)), 0));
  }
  if (order == MemoryOrder::Acquire || order == MemoryOrder::SeqCst || order == MemoryOrder::AcqRel) {
    _ReadWriteBarrier();
  }
  return value;
}

template<typename T>
inline void atomic_store(T* ptr, T value, MemoryOrder order) {
  static_assert(sizeof(T) <= 8, "Atomic operations only support types up to 64 bits");
  if (order == MemoryOrder::Release || order == MemoryOrder::SeqCst || order == MemoryOrder::AcqRel) {
    _ReadWriteBarrier();
  }
  if constexpr (sizeof(T) == 1) {
    _InterlockedExchange8(reinterpret_cast<volatile char*>(ptr), static_cast<char>(value));
  } else if constexpr (sizeof(T) == 2) {
    _InterlockedExchange16(reinterpret_cast<volatile short*>(ptr), static_cast<short>(value));
  } else if constexpr (sizeof(T) == 4) {
    _InterlockedExchange(reinterpret_cast<volatile long*>(ptr), static_cast<long>(value));
  } else {
    _InterlockedExchange64(reinterpret_cast<volatile long long*>(ptr), static_cast<long long>(value));
  }
}

template<typename T>
inline T atomic_exchange(T* ptr, T value, MemoryOrder order) {
  static_assert(sizeof(T) <= 8, "Atomic operations only support types up to 64 bits");
  if (order == MemoryOrder::Release || order == MemoryOrder::SeqCst || order == MemoryOrder::AcqRel) {
    _ReadWriteBarrier();
  }
  T old_val;
  if constexpr (sizeof(T) == 1) {
    old_val = static_cast<T>(_InterlockedExchange8(reinterpret_cast<volatile char*>(ptr), static_cast<char>(value)));
  } else if constexpr (sizeof(T) == 2) {
    old_val = static_cast<T>(_InterlockedExchange16(reinterpret_cast<volatile short*>(ptr), static_cast<short>(value)));
  } else if constexpr (sizeof(T) == 4) {
    old_val = static_cast<T>(_InterlockedExchange(reinterpret_cast<volatile long*>(ptr), static_cast<long>(value)));
  } else {
    old_val = static_cast<T>(_InterlockedExchange64(reinterpret_cast<volatile long long*>(ptr), static_cast<long long>(value)));
  }
  return old_val;
}

template<typename T>
inline T atomic_fetch_add(T* ptr, T value, MemoryOrder /*order*/) {
  static_assert(sizeof(T) <= 8, "Atomic operations only support types up to 64 bits");
  if constexpr (sizeof(T) == 1) {
    return static_cast<T>(_InterlockedExchangeAdd8(reinterpret_cast<volatile char*>(ptr), static_cast<char>(value)));
  } else if constexpr (sizeof(T) == 2) {
    return static_cast<T>(_InterlockedExchangeAdd16(reinterpret_cast<volatile short*>(ptr), static_cast<short>(value)));
  } else if constexpr (sizeof(T) == 4) {
    return static_cast<T>(_InterlockedExchangeAdd(reinterpret_cast<volatile long*>(ptr), static_cast<long>(value)));
  } else {
    return static_cast<T>(_InterlockedExchangeAdd64(reinterpret_cast<volatile long long*>(ptr), static_cast<long long>(value)));
  }
}

template<typename T>
inline T atomic_fetch_sub(T* ptr, T value, MemoryOrder order) {
  // Subtraction is addition of negated value
  return atomic_fetch_add(ptr, static_cast<T>(-static_cast<std::make_signed_t<T>>(value)), order);
}

template<typename T>
inline T atomic_xor_fetch(T* ptr, T value, MemoryOrder /*order*/) {
  static_assert(sizeof(T) <= 8, "Atomic operations only support types up to 64 bits");
  T old_val;
  if constexpr (sizeof(T) == 1) {
    old_val = static_cast<T>(_InterlockedXor8(reinterpret_cast<volatile char*>(ptr), static_cast<char>(value)));
  } else if constexpr (sizeof(T) == 2) {
    old_val = static_cast<T>(_InterlockedXor16(reinterpret_cast<volatile short*>(ptr), static_cast<short>(value)));
  } else if constexpr (sizeof(T) == 4) {
    old_val = static_cast<T>(_InterlockedXor(reinterpret_cast<volatile long*>(ptr), static_cast<long>(value)));
  } else {
    old_val = static_cast<T>(_InterlockedXor64(reinterpret_cast<volatile long long*>(ptr), static_cast<long long>(value)));
  }
  return old_val ^ value;  // Return new value (after XOR)
}

template<typename T>
inline bool atomic_compare_exchange(T* ptr, T* expected, T desired,
                                    MemoryOrder /*success_order*/, MemoryOrder /*failure_order*/) {
  static_assert(sizeof(T) <= 8, "Atomic operations only support types up to 64 bits");
  T old_val;
  if constexpr (sizeof(T) == 1) {
    old_val = static_cast<T>(_InterlockedCompareExchange8(
        reinterpret_cast<volatile char*>(ptr),
        static_cast<char>(desired),
        static_cast<char>(*expected)));
  } else if constexpr (sizeof(T) == 2) {
    old_val = static_cast<T>(_InterlockedCompareExchange16(
        reinterpret_cast<volatile short*>(ptr),
        static_cast<short>(desired),
        static_cast<short>(*expected)));
  } else if constexpr (sizeof(T) == 4) {
    old_val = static_cast<T>(_InterlockedCompareExchange(
        reinterpret_cast<volatile long*>(ptr),
        static_cast<long>(desired),
        static_cast<long>(*expected)));
  } else {
    old_val = static_cast<T>(_InterlockedCompareExchange64(
        reinterpret_cast<volatile long long*>(ptr),
        static_cast<long long>(desired),
        static_cast<long long>(*expected)));
  }
  if (old_val == *expected) {
    return true;
  } else {
    *expected = old_val;
    return false;
  }
}

#else  // GCC/Clang

// GCC/Clang implementations using __atomic_* intrinsics

constexpr int to_gcc_order(MemoryOrder order) {
  switch (order) {
    case MemoryOrder::Relaxed: return __ATOMIC_RELAXED;
    case MemoryOrder::Acquire: return __ATOMIC_ACQUIRE;
    case MemoryOrder::Release: return __ATOMIC_RELEASE;
    case MemoryOrder::AcqRel:  return __ATOMIC_ACQ_REL;
    case MemoryOrder::SeqCst:  return __ATOMIC_SEQ_CST;
  }
  return __ATOMIC_SEQ_CST;
}

template<typename T>
inline T atomic_load(const T* ptr, MemoryOrder order) {
  return __atomic_load_n(ptr, to_gcc_order(order));
}

template<typename T>
inline void atomic_store(T* ptr, T value, MemoryOrder order) {
  __atomic_store_n(ptr, value, to_gcc_order(order));
}

template<typename T>
inline T atomic_exchange(T* ptr, T value, MemoryOrder order) {
  return __atomic_exchange_n(ptr, value, to_gcc_order(order));
}

template<typename T>
inline T atomic_fetch_add(T* ptr, T value, MemoryOrder order) {
  return __atomic_fetch_add(ptr, value, to_gcc_order(order));
}

template<typename T>
inline T atomic_fetch_sub(T* ptr, T value, MemoryOrder order) {
  return __atomic_fetch_sub(ptr, value, to_gcc_order(order));
}

template<typename T>
inline T atomic_xor_fetch(T* ptr, T value, MemoryOrder order) {
  return __atomic_xor_fetch(ptr, value, to_gcc_order(order));
}

template<typename T>
inline bool atomic_compare_exchange(T* ptr, T* expected, T desired,
                                    MemoryOrder success_order, MemoryOrder failure_order) {
  return __atomic_compare_exchange(ptr, expected, &desired, false,
                                   to_gcc_order(success_order), to_gcc_order(failure_order));
}

#endif  // _MSC_VER

}  // namespace detail

// Public API - convenience macros/functions that match the original __atomic_* pattern

template<typename T>
inline T mtk_atomic_load(const T* ptr, MemoryOrder order = MemoryOrder::SeqCst) {
  return detail::atomic_load(ptr, order);
}

template<typename T>
inline void mtk_atomic_store(T* ptr, T value, MemoryOrder order = MemoryOrder::SeqCst) {
  detail::atomic_store(ptr, value, order);
}

template<typename T>
inline T mtk_atomic_exchange(T* ptr, T value, MemoryOrder order = MemoryOrder::SeqCst) {
  return detail::atomic_exchange(ptr, value, order);
}

template<typename T>
inline T mtk_atomic_fetch_add(T* ptr, T value, MemoryOrder order = MemoryOrder::SeqCst) {
  return detail::atomic_fetch_add(ptr, value, order);
}

template<typename T>
inline T mtk_atomic_fetch_sub(T* ptr, T value, MemoryOrder order = MemoryOrder::SeqCst) {
  return detail::atomic_fetch_sub(ptr, value, order);
}

template<typename T>
inline T mtk_atomic_xor_fetch(T* ptr, T value, MemoryOrder order = MemoryOrder::SeqCst) {
  return detail::atomic_xor_fetch(ptr, value, order);
}

template<typename T>
inline bool mtk_atomic_compare_exchange(T* ptr, T* expected, T desired,
                                        MemoryOrder success_order = MemoryOrder::SeqCst,
                                        MemoryOrder failure_order = MemoryOrder::Relaxed) {
  return detail::atomic_compare_exchange(ptr, expected, desired, success_order, failure_order);
}

// Convenience overload for compare_exchange_n pattern (value instead of pointer for expected)
template<typename T>
inline bool mtk_atomic_compare_exchange_n(T* ptr, T expected, T desired,
                                          MemoryOrder success_order = MemoryOrder::SeqCst,
                                          MemoryOrder failure_order = MemoryOrder::Relaxed) {
  return detail::atomic_compare_exchange(ptr, &expected, desired, success_order, failure_order);
}

}  // namespace mt_kahypar

#ifdef _MSC_VER
#ifndef MT_KAHYPAR_GCC_ATOMIC_COMPAT
#define MT_KAHYPAR_GCC_ATOMIC_COMPAT

// GCC Atomic Memory Orders
#define __ATOMIC_RELAXED 0
#define __ATOMIC_CONSUME 1
#define __ATOMIC_ACQUIRE 2
#define __ATOMIC_RELEASE 3
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_SEQ_CST 5

// Helper to map integer order to enum (simplified)
inline mt_kahypar::MemoryOrder mtk_map_gcc_order(int order) {
    switch(order) {
        case __ATOMIC_RELAXED: return mt_kahypar::MemoryOrder::Relaxed;
        case __ATOMIC_ACQUIRE: return mt_kahypar::MemoryOrder::Acquire;
        case __ATOMIC_RELEASE: return mt_kahypar::MemoryOrder::Release;
        case __ATOMIC_ACQ_REL: return mt_kahypar::MemoryOrder::AcqRel;
        case __ATOMIC_SEQ_CST: return mt_kahypar::MemoryOrder::SeqCst;
        default: return mt_kahypar::MemoryOrder::SeqCst;
    }
}

// GCC Atomic Builtin Shims
template<typename T, typename U>
inline T __atomic_fetch_add(T* ptr, U val, int memorder) {
    return mt_kahypar::mtk_atomic_fetch_add(ptr, static_cast<T>(val), mtk_map_gcc_order(memorder));
}

template<typename T, typename U>
inline T __atomic_fetch_sub(T* ptr, U val, int memorder) {
    return mt_kahypar::mtk_atomic_fetch_sub(ptr, static_cast<T>(val), mtk_map_gcc_order(memorder));
}

template<typename T, typename U>
inline T __atomic_exchange_n(T* ptr, U val, int memorder) {
    return mt_kahypar::mtk_atomic_exchange(ptr, static_cast<T>(val), mtk_map_gcc_order(memorder));
}

template<typename T, typename U>
inline bool __atomic_compare_exchange_n(T* ptr, T* expected, U desired, bool /*weak*/, int success_order, int failure_order) {
    return mt_kahypar::mtk_atomic_compare_exchange(ptr, expected, static_cast<T>(desired), mtk_map_gcc_order(success_order), mtk_map_gcc_order(failure_order));
}

#endif
#endif
