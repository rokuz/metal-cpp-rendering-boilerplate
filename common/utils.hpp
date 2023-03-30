// Copyright © 2022 Roman Kuznetsov.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <simd/simd.h>

#include <cstdint>
#include <iostream>
#include <vector>

// RAII for releasing metal-cpp objects.
template <class T> class metal_guard {
public:
  explicit metal_guard(T *val) : m_value(val) {}
  ~metal_guard() {
    if (m_value)
      m_value->release();
  }

private:
  T *m_value = nullptr;
};

#define METAL_GUARD(value) metal_guard guard_##value(value);

// Converts C-string to metal-cpp compatible string.
#define STR(str) NS::String::string(str, NS::StringEncoding::UTF8StringEncoding)

// Converts metal-cpp string to C-string.
#define C_STR(str) str->utf8String()

// Boilerplate: check NS::Error and return.
#define CHECK_AND_RETURN(error, returnVal)                                     \
  if (error) {                                                                 \
    std::cerr << "Error: " << error->localizedDescription()->utf8String()      \
              << std::endl;                                                    \
    error->release();                                                          \
    return returnVal;                                                          \
  }

// Vector helpers.
inline packed_float2 make_packed_float2(glm::vec2 const &v) {
  packed_float2 out;
  out.x = v.x;
  out.y = v.y;
  return out;
}

inline packed_float4 make_packed_float4(glm::vec4 const &v) {
  packed_float4 out;
  out.x = v.x;
  out.y = v.y;
  out.z = v.z;
  out.w = v.w;
  return out;
}

namespace utils {
// Aligns value to the specified alignment in bytes.
inline uint32_t getAligned(uint32_t bytes, uint32_t alignment) {
  return (bytes + (alignment - 1)) & ~(alignment - 1);
}

// Combines hashes.
// https://github.com/boostorg/container_hash/blob/develop/include/boost/container_hash/hash.hpp
template <class T> inline void hashCombine(std::size_t &seed, T const &v) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace utils
