/*******************************************************************************
    Taichi - Physically based Computer Graphics Library

    Copyright (c) 2016 Yuanming Hu <yuanmhu@gmail.com>

    All rights reserved. Use of this source code is governed by
    the MIT license as written in the LICENSE file.
*******************************************************************************/

#pragma once

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <algorithm>

// Do not disable assert...
#ifdef NDEBUG
#undef NDEBUG
#endif

#ifdef _WIN64
#define __FILENAME__ \
  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#pragma warning(push)
#pragma warning(disable : 4005)
#include <windows.h>
#pragma warning(pop)
#include <intrin.h>
#define TC_EXPORT __declspec(dllexport)
#else
#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define TC_EXPORT
#endif
#define PRINT(x)                                         \
  {                                                      \
    printf("%s[%d]: %s = ", __FILENAME__, __LINE__, #x); \
    taichi::print(x);                                    \
  };
#define P(x) PRINT(x)

#ifndef _WIN64
#define sscanf_s sscanf
#define sprintf_s sprintf
#endif

#undef assert
#ifdef _WIN64
#ifndef TC_PASS_EXCEPTION_TO_PYTHON
// For Visual Studio debugging...
#define DEBUG_TRIGGER __debugbreak()
#else
#define DEBUG_TRIGGER
#endif
#else
#define DEBUG_TRIGGER
#endif
#define assert(x)                                                            \
  {                                                                          \
    bool ret = static_cast<bool>(x);                                         \
    if (!ret) {                                                              \
      printf("%s@(Ln %d): Assertion Failed. [%s]\n", __FILENAME__, __LINE__, \
             #x);                                                            \
      std::cout << std::flush;                                               \
      print_traceback();                                                     \
      DEBUG_TRIGGER;                                                         \
      taichi_raise_assertion_failure_in_python("Assertion failed.");         \
    }                                                                        \
  }
#define assert_info(x, info)                                                 \
  {                                                                          \
    bool ___ret___ = static_cast<bool>(x);                                   \
    if (!___ret___) {                                                        \
      printf("%s@(Ln %d): Assertion Failed. [%s]\n", __FILENAME__, __LINE__, \
             &((info)[0]));                                                  \
      std::cout << std::flush;                                               \
      print_traceback();                                                     \
      DEBUG_TRIGGER;                                                         \
      taichi_raise_assertion_failure_in_python("Assertion failed.");         \
    }                                                                        \
  }
#define error(info) assert_info(false, info)
#define NOT_IMPLEMENTED assert_info(false, "Not Implemented!");

#define TC_NAMESPACE_BEGIN namespace taichi {
#define TC_NAMESPACE_END }

// Check for inf, nan?
// #define CV_ON

#ifdef CV_ON
#define CV(v)                                              \
  if (abnormal(v)) {                                       \
    for (int i = 0; i < 1; i++)                            \
      printf("Abnormal value %s (Ln %d)\n", #v, __LINE__); \
    taichi::print(v);                                      \
    puts("");                                              \
  }
#else
#define CV(v)
#endif

TC_EXPORT void taichi_raise_assertion_failure_in_python(const char *msg);

TC_NAMESPACE_BEGIN

using int8 = signed char;
using uint8 = unsigned char;

using int16 = short;
using uint16 = unsigned short;

using int32 = int;
using uint32 = unsigned int;

#ifdef _WIN64
using int64 = __int64;
using uint64 = unsigned __int64;
#define TC_FORCE_INLINE __forceinline
#else
using int64 = long long;
using uint64 = unsigned long long;
#define TC_FORCE_INLINE __attribute__((always_inline))
#endif

using float32 = float;
using float64 = double;

#ifdef TC_USE_DOUBLE
using real = float64;
#else
using real = float32;
#endif

// Float literal for both float32/64
// (Learned from https://github.com/hi2p-perim/lightmetrica-v2)
real constexpr operator"" _f(long double v) { return real(v); }
real constexpr operator"" _f(unsigned long long v) { return real(v); }

float32 constexpr operator"" _f32(long double v) { return float32(v); }
float32 constexpr operator"" _f32(unsigned long long v) { return float32(v); }

float32 constexpr operator"" _fs(long double v) { return float32(v); }
float32 constexpr operator"" _fs(unsigned long long v) { return float32(v); }

float64 constexpr operator"" _f64(long double v) { return float64(v); }
float64 constexpr operator"" _f64(unsigned long long v) { return float64(v); }

float64 constexpr operator"" _fd(long double v) { return float64(v); }
float64 constexpr operator"" _fd(unsigned long long v) { return float64(v); }


TC_EXPORT void print_traceback();

template <typename T>
inline void print(T t) {
  std::cout << t << std::endl;
}

namespace STATIC_IF {
// reference: https://github.com/wichtounet/cpp_utils

struct identity {
  template <typename T>
  T operator()(T &&x) const {
    return std::forward<T>(x);
  }
};

template <bool Cond>
struct statement {
  template <typename F>
  void then(const F &f) {
    f(identity());
  }

  template <typename F>
  void else_(const F &) {}
};

template <>
struct statement<false> {
  template <typename F>
  void then(const F &) {}

  template <typename F>
  void else_(const F &f) {
    f(identity());
  }
};

template <bool Cond, typename F>
inline statement<Cond> static_if(F const &f) {
  statement<Cond> if_;
  if_.then(f);
  return if_;
}
}

using STATIC_IF::static_if;

#define TC_STATIC_IF(x) static_if<(x)>([&](const auto& _____) {
#define TC_STATIC_ELSE \
  }).else_([&](const auto &_____) {
#define TC_STATIC_END_IF \
  });

// After we switch to C++17, we should use
// (Note the the behaviour of 'return' is still different.)

/*
#define TC_STATIC_IF(x) if constexpr(x) {
#define TC_STATIC_ELSE \
    } else {
#define TC_STATIC_END_IF \
    }
*/

TC_NAMESPACE_END
