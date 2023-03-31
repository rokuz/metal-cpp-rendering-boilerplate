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

#include <functional>
#include <memory>
#include <string>

class main_thread {
public:
  static void dispatch(std::function<void()> f);
};

class worker_thread_impl;

class worker_thread {
public:
  explicit worker_thread(std::string const & threadName = "");
  ~worker_thread();
  void dispatch(std::function<void()> f);

private:
  std::unique_ptr<worker_thread_impl> const m_impl;
};
