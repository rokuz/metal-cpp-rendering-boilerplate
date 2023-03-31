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

#include "threads.hpp"

#include <dispatch/dispatch.h>

#if !__has_feature(objc_arc)
#error "ARC is off"
#endif

// static
void main_thread::dispatch(std::function<void()> f) {
  dispatch_async(dispatch_get_main_queue(), ^{
    f();
  });
}

class worker_thread_impl {
public:
  worker_thread_impl(std::string const & threadName) {
    m_dispatchQueue = dispatch_queue_create(threadName.empty() ? nullptr : threadName.c_str(),
                                            DISPATCH_QUEUE_CONCURRENT);
  }

  void dispatch(std::function<void()> f) {
    dispatch_async(m_dispatchQueue, ^{
      f();
    });
  }

private:
  dispatch_queue_t m_dispatchQueue;
};

worker_thread::worker_thread(std::string const & threadName /* = "" */)
  : m_impl(std::make_unique<worker_thread_impl>(threadName)) {}

worker_thread::~worker_thread() = default;

void worker_thread::dispatch(std::function<void()> f) { m_impl->dispatch(std::move(f)); }
