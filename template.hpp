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

#include "common/app.hpp"

#include <cstdint>
#include <string>

class Renderer : public App {
public:
  Renderer();

  char const * const getName() const override;

  bool onInitialize(MTL::Device * const device,
                    MTL::CommandQueue * const commandQueue,
                    uint32_t screenWidth,
                    uint32_t screenHeight) override;
  void onDeinitialize() override;

  void onResize(uint32_t screenWidth, uint32_t screenHeight) override;

  void renderFrame(MTL::CommandBuffer * frameCommandBuffer,
                   MTL::Texture * outputTexture,
                   double elapsedSeconds) override;

private:
  struct MetalContext {
    MTL::Device * const m_device;
    MTL::CommandQueue * const m_commandQueue;
  };
  std::unique_ptr<MetalContext> m_context;
  uint32_t m_screenWidth = 0;
  uint32_t m_screenHeight = 0;

  // Info & Controls.
  std::string m_gpuFamily;
  double m_fpsTimer = 0.0;
  uint32_t m_frameCounter = 0;
  double m_fps = 0.0;
};
