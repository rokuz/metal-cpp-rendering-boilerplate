// Copyright © 2023 Roman Kuznetsov.
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

#include <Metal/Metal.hpp>
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

#include "glm_math.hpp"

// RAII for releasing metal-cpp objects.
template <class T>
class metal_guard {
public:
  explicit metal_guard(T * val) : m_value(val) {}
  ~metal_guard() {
    if (m_value) m_value->release();
  }

private:
  T * m_value = nullptr;
};

#define METAL_GUARD(value) metal_guard guard_##value(value);

// Converts C-string to metal-cpp compatible string.
#define STR(str) NS::String::string(str, NS::StringEncoding::UTF8StringEncoding)

// Converts metal-cpp string to C-string.
#define C_STR(str) str->utf8String()

// Boilerplate: check NS::Error and return.
#define CHECK_AND_RETURN(error, returnVal)                                              \
  if (error) {                                                                          \
    std::cerr << "Error: " << error->localizedDescription()->utf8String() << std::endl; \
    error->release();                                                                   \
    return returnVal;                                                                   \
  }

// Assert definition.
#define METAL_ASSERT(condition)                         \
  if (!(condition)) {                                   \
    std::cerr << "Assert: " << #condition << std::endl; \
    assert(condition);                                  \
  }

// Vector helpers.
inline packed_float2 make_packed_float2(glm::vec2 const & v) {
  packed_float2 out;
  out.x = v.x;
  out.y = v.y;
  return out;
}

inline packed_float4 make_packed_float4(glm::vec4 const & v) {
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
// Based on:
// https://github.com/boostorg/container_hash/blob/develop/include/boost/container_hash/hash.hpp
inline void hashCombine(std::size_t & seed) {}

template <typename Arg, typename... Args>
inline void hashCombine(std::size_t & seed, Arg const & first, Args const &... rest) {
  std::hash<Arg> hasher;
  seed ^= hasher(first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hashCombine(seed, rest...);
}

// Returns GPU Family from Metal device.
inline std::string getMetalGpuFamily(MTL::Device * const device) {
  static std::array families{
    std::pair{MTL::GPUFamilyMetal3, "Metal 3"},
    std::pair{MTL::GPUFamilyMacCatalyst2, "Mac Catalyst 2"},
    std::pair{MTL::GPUFamilyMacCatalyst1, "Mac Catalyst 1"},
    std::pair{MTL::GPUFamilyCommon3, "Common 3"},
    std::pair{MTL::GPUFamilyCommon2, "Common 2"},
    std::pair{MTL::GPUFamilyCommon1, "Common 1"},
    std::pair{MTL::GPUFamilyMac2, "Mac 2"},
    std::pair{MTL::GPUFamilyMac1, "Mac 1"},
    std::pair{MTL::GPUFamilyApple8, "Apple 8"},
    std::pair{MTL::GPUFamilyApple7, "Apple 7"},
    std::pair{MTL::GPUFamilyApple6, "Apple 6"},
    std::pair{MTL::GPUFamilyApple5, "Apple 5"},
    std::pair{MTL::GPUFamilyApple4, "Apple 4"},
    std::pair{MTL::GPUFamilyApple3, "Apple 3"},
    std::pair{MTL::GPUFamilyApple2, "Apple 2"},
    std::pair{MTL::GPUFamilyApple1, "Apple 1"},
  };
  for (auto & [family, name] : families) {
    if (device->supportsFamily(family)) return name;
  }
  return "";
}
}  // namespace utils
