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

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include <cstdint>
#include <functional>

class App {
public:
  virtual ~App() = default;

  virtual char const *const getName() const {
    static char const *const kName = "Metal 3 Demo";
    return kName;
  }

  virtual bool onInitialize(MTL::Device *device, uint32_t screenWidth,
                            uint32_t screenHeight) = 0;
  virtual void onDeinitialize() = 0;
  virtual void onMainLoopTick(CA::MetalDrawable *drawable,
                              double elapsedSeconds) {}
  virtual void onResize(uint32_t screenWidth, uint32_t screenHeight) {}

  virtual void onKeyButton(int key, int scancode, bool pressed) {}
  virtual void onMouseButton(double xpos, double ypos, int button,
                             bool pressed) {}
  virtual void onMouseMove(double xpos, double ypos) {}
};

extern void closeApp();
