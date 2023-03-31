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

#include "renderer.hpp"

#include <chrono>

#include "common/utils.hpp"

App * getApp() {
  static Renderer app;
  return &app;
}

char const * const kDemoName = "New fancy Metal demo";

Renderer::Renderer() {}

char const * const Renderer::getName() const { return kDemoName; }

bool Renderer::onInitialize(MTL::Device * const device,
                            MTL::CommandQueue * const commandQueue,
                            uint32_t screenWidth,
                            uint32_t screenHeight) {
  METAL_ASSERT(device != 0);
  METAL_ASSERT(commandQueue != 0);
  m_context = std::unique_ptr<MetalContext>(new MetalContext{device, commandQueue});
  m_screenWidth = screenWidth;
  m_screenHeight = screenHeight;
  m_gpuFamily = utils::getMetalGpuFamily(device);

  return true;
}

void Renderer::onDeinitialize() {
  auto commandBuffer = m_context->m_commandQueue->commandBuffer();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();
}

void Renderer::onResize(uint32_t screenWidth, uint32_t screenHeight) {
  m_screenWidth = screenWidth;
  m_screenHeight = screenHeight;
}

void Renderer::renderFrame(MTL::CommandBuffer * frameCommandBuffer,
                           MTL::Texture * outputTexture,
                           double elapsedSeconds) {
  NS::AutoreleasePool * autoreleasePool = NS::AutoreleasePool::alloc()->init();
  METAL_GUARD(autoreleasePool);

  auto renderPassDescriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
  auto colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
  colorAttachment->setTexture(outputTexture);
  colorAttachment->setClearColor(MTL::ClearColor::Make(0.1, 0.1, 0.1, 1.0));
  colorAttachment->setLoadAction(MTL::LoadAction::LoadActionClear);
  colorAttachment->setStoreAction(MTL::StoreAction::StoreActionStore);

  MTL::RenderCommandEncoder * encoder =
    frameCommandBuffer->renderCommandEncoder(renderPassDescriptor);
  encoder->setLabel(STR("Main Command Encoder"));

  // TODO: put your rendering code here.

  app::renderImGui(frameCommandBuffer, renderPassDescriptor, encoder, [=, this](ImGuiIO & io) {
    m_fpsTimer += (1.0 / io.Framerate);
    m_frameCounter++;
    if (m_fpsTimer > 1.0) {
      m_fps = static_cast<double>(m_frameCounter) / m_fpsTimer;
      m_fpsTimer = 0;
      m_frameCounter = 0;
    }

    static bool enableVSync = app::isEnabledVSync();
    ImVec2 sz = ImGui::GetMainViewport()->WorkSize;
    ImGui::SetNextWindowPos(ImVec2(sz.x - 10, 10), ImGuiCond_Appearing, ImVec2(1.0f, 0.0f));
    ImGui::Begin("Info & Controls");
    ImGui::Text("Device: %s", m_context->m_device->name()->utf8String());
    ImGui::Text("GPU Family: %s", m_gpuFamily.c_str());
    ImGui::Text("Avg time frame = %.3f ms (%.1f FPS)",
                m_fps == 0 ? 0.0f : (1000.0f / m_fps),
                m_fps);
    if (ImGui::Checkbox("Enable VSync", &enableVSync)) {
      app::setEnabledVSync(enableVSync);
    }
    if (ImGui::IsKeyReleased(ImGuiKey_Escape)) {
      app::closeApp();
    }
    ImGui::End();
  });
  encoder->endEncoding();
}
