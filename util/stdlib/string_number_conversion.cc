// Copyright 2014 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "util/stdlib/string_number_conversion.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <limits>

#include "base/basictypes.h"
#include "base/logging.h"
#include "util/stdlib/cxx.h"

// CONSTEXPR_COMPILE_ASSERT will be a normal COMPILE_ASSERT if the C++ library
// is the C++11 library. If using an older C++ library when compiling C++11
// code, the std::numeric_limits<>::min() and max() functions will not be
// marked as constexpr, and thus won’t be usable with C++11’s static_assert().
// In that case, a run-time CHECK() will have to do.
#if CXX_LIBRARY_VERSION >= 2011
#define CONSTEXPR_COMPILE_ASSERT(condition, message) \
  COMPILE_ASSERT(condition, message)
#else
#define CONSTEXPR_COMPILE_ASSERT(condition, message) CHECK(condition)
#endif

namespace {

template <typename TIntType, typename TLongType>
struct StringToIntegerTraits {
  typedef TIntType IntType;
  typedef TLongType LongType;
  static void TypeCheck() {
    COMPILE_ASSERT(std::numeric_limits<TIntType>::is_integer &&
                       std::numeric_limits<TLongType>::is_integer,
                   IntType_and_LongType_must_be_integer);
    COMPILE_ASSERT(std::numeric_limits<TIntType>::is_signed ==
                       std::numeric_limits<TLongType>::is_signed,
                   IntType_and_LongType_signedness_must_agree);
    CONSTEXPR_COMPILE_ASSERT(std::numeric_limits<TIntType>::min() >=
                                     std::numeric_limits<TLongType>::min() &&
                                 std::numeric_limits<TIntType>::min() <
                                     std::numeric_limits<TLongType>::max(),
                             IntType_min_must_be_in_LongType_range);
    CONSTEXPR_COMPILE_ASSERT(std::numeric_limits<TIntType>::max() >
                                     std::numeric_limits<TLongType>::min() &&
                                 std::numeric_limits<TIntType>::max() <=
                                     std::numeric_limits<TLongType>::max(),
                             IntType_max_must_be_in_LongType_range);
  }
};

template <typename TIntType, typename TLongType>
struct StringToSignedIntegerTraits
    : public StringToIntegerTraits<TIntType, TLongType> {
  static void TypeCheck() {
    COMPILE_ASSERT(std::numeric_limits<TIntType>::is_signed,
                   StringToSignedTraits_IntType_must_be_signed);
    return super::TypeCheck();
  }
  static bool IsNegativeOverflow(TLongType value) {
    return value < std::numeric_limits<TIntType>::min();
  }

 private:
  typedef StringToIntegerTraits<TIntType, TLongType> super;
};

template <typename TIntType, typename TLongType>
struct StringToUnsignedIntegerTraits
    : public StringToIntegerTraits<TIntType, TLongType> {
  static void TypeCheck() {
    COMPILE_ASSERT(!std::numeric_limits<TIntType>::is_signed,
                   StringToUnsignedTraits_IntType_must_be_unsigned);
    return super::TypeCheck();
  }
  static bool IsNegativeOverflow(TLongType value) { return false; }

 private:
  typedef StringToIntegerTraits<TIntType, TLongType> super;
};

struct StringToIntTraits : public StringToSignedIntegerTraits<int, long> {
  static LongType Convert(const char* str, char** end, int base) {
    return strtol(str, end, base);
  }
};

struct StringToUnsignedIntTraits
    : public StringToUnsignedIntegerTraits<unsigned int, unsigned long> {
  static LongType Convert(const char* str, char** end, int base) {
    if (str[0] == '-') {
      return 0;
    }
    return strtoul(str, end, base);
  }
};

template <typename Traits>
bool StringToIntegerInternal(const base::StringPiece& string,
                             typename Traits::IntType* number) {
  typedef typename Traits::IntType IntType;
  typedef typename Traits::LongType LongType;

  Traits::TypeCheck();

  if (string.empty() || isspace(string[0])) {
    return false;
  }

  if (string[string.length()] != '\0') {
    // The implementations use the C standard library’s conversion routines,
    // which rely on the strings having a trailing NUL character. std::string
    // will NUL-terminate.
    std::string terminated_string(string.data(), string.length());
    return StringToIntegerInternal<Traits>(terminated_string, number);
  }

  errno = 0;
  char* end;
  LongType result = Traits::Convert(string.data(), &end, 0);
  if (Traits::IsNegativeOverflow(result) ||
      result > std::numeric_limits<IntType>::max() ||
      errno == ERANGE ||
      end != string.data() + string.length()) {
    return false;
  }
  *number = result;
  return true;
}

}  // namespace

namespace crashpad {

bool StringToNumber(const base::StringPiece& string, int* number) {
  return StringToIntegerInternal<StringToIntTraits>(string, number);
}

bool StringToNumber(const base::StringPiece& string, unsigned int* number) {
  return StringToIntegerInternal<StringToUnsignedIntTraits>(string, number);
}

}  // namespace crashpad
