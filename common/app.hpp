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

#include <imgui.h>

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include <cstdint>
#include <functional>

#include "glm_math.hpp"

class App {
public:
  virtual ~App() = default;

  [[nodiscard]] virtual char const * const getName() const {
    static char const * const kName = "Metal 3 Demo";
    return kName;
  }

  [[nodiscard]] virtual bool onInitialize(MTL::Device * const device,
                                          MTL::CommandQueue * const commandQueue,
                                          uint32_t screenWidth,
                                          uint32_t screenHeight) = 0;

  virtual void onDeinitialize() {}

  virtual void renderFrame(MTL::CommandBuffer * frameCommandBuffer,
                           MTL::Texture * outputTexture,
                           double elapsedSeconds) {}

  virtual void onResize(uint32_t screenWidth, uint32_t screenHeight) {}
};

namespace app {
extern void closeApp();
extern void setEnabledVSync(bool enabled);
extern bool isEnabledVSync();

using ImGuiBuilder = std::function<void(ImGuiIO &)>;
extern void renderImGui(MTL::CommandBuffer * frameCommandBuffer,
                        MTL::RenderPassDescriptor * rpDescriptor,
                        MTL::RenderCommandEncoder * encoder,
                        ImGuiBuilder && builder);
}  // namespace app
