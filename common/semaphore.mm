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

#include "semaphore.hpp"

#include <dispatch/dispatch.h>

#if !__has_feature(objc_arc)
#error "ARC is off"
#endif

class semaphore_impl {
public:
  explicit semaphore_impl(uint32_t max_value) {
    m_semaphore = dispatch_semaphore_create(max_value);
  }

  void signal() const { dispatch_semaphore_signal(m_semaphore); }

  void wait() const { dispatch_semaphore_wait(m_semaphore, DISPATCH_TIME_FOREVER); }

private:
  dispatch_semaphore_t m_semaphore;
};

semaphore::semaphore(uint32_t max_value)
  : m_impl(std::make_unique<semaphore_impl>(max_value)), m_max(max_value) {}

semaphore::~semaphore() = default;

void semaphore::signal() const { m_impl->signal(); }

void semaphore::wait() const { m_impl->wait(); }
