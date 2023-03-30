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

#include <cstdint>
#include <memory>

class semaphore_impl;

class semaphore {
public:
  explicit semaphore(uint32_t max_value);
  ~semaphore();
  void signal();
  void wait();
  uint32_t max() const { return m_max; }

private:
  std::unique_ptr<semaphore_impl> const m_impl;
  uint32_t const m_max;
};
