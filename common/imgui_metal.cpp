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

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_metal.h>
#include <imgui.h>

#include <Metal/Metal.hpp>

#include "app.hpp"
#include "utils.hpp"

namespace app {
void renderImGui(MTL::CommandBuffer * frameCommandBuffer,
                 MTL::RenderPassDescriptor * rpDescriptor,
                 MTL::RenderCommandEncoder * encoder,
                 ImGuiBuilder && builder) {
  encoder->pushDebugGroup(STR("Encode ImGui"));

  ImGui_ImplMetal_NewFrame(rpDescriptor);
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGuiIO & io = ImGui::GetIO();
  builder(io);
  ImGui::Render();
  ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), frameCommandBuffer, encoder);

  encoder->popDebugGroup();
}
}  // namespace app
