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

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_metal.h>
#include <imgui.h>

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <chrono>
#include <cxxopts.hpp>
#include <iostream>

#include "app.hpp"
#include "semaphore.hpp"

#if !__has_feature(objc_arc)
#error "ARC is off"
#endif

// This function must be defined in an external module.
extern App * getApp();

uint32_t constexpr kMaxFramesInFlight = 3;

GLFWwindow * window = nullptr;
bool needClose = false;

namespace app {
void closeApp() { needClose = true; }

void setEnabledVSync(bool enabled) {
  NSWindow * nswindow = glfwGetCocoaWindow(window);
  CAMetalLayer * layer = (CAMetalLayer *)nswindow.contentView.layer;
  layer.displaySyncEnabled = enabled;
}

bool isEnabledVSync() {
  NSWindow * nswindow = glfwGetCocoaWindow(window);
  CAMetalLayer * layer = (CAMetalLayer *)nswindow.contentView.layer;
  return layer.displaySyncEnabled;
}
}  // namespace app

void resizeCallback(GLFWwindow * window, int, int) {
  int fbWidth = 0, fbHeight = 0;
  glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
  getApp()->onResize(static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight));
}

void errorCallback(int error, const char * description) {
  std::cerr << "Error (" << error << "): " << description << std::endl;
}

// Hacky way to get a monitor on what window is opened.
// https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
GLFWmonitor * getCurrentMonitor(GLFWwindow * window) {
  int wx = 0, wy = 0, ww = 0, wh = 0;
  int mx = 0, my = 0, mw = 0, mh = 0;
  int overlap, bestoverlap = 0;
  GLFWmonitor * bestmonitor = nullptr;
  const GLFWvidmode * mode = nullptr;

  glfwGetWindowPos(window, &wx, &wy);
  glfwGetWindowSize(window, &ww, &wh);

  int nmonitors = 0;
  auto monitors = glfwGetMonitors(&nmonitors);

  for (int i = 0; i < nmonitors; ++i) {
    mode = glfwGetVideoMode(monitors[i]);
    glfwGetMonitorPos(monitors[i], &mx, &my);
    mw = mode->width;
    mh = mode->height;

    overlap = std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
              std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

    if (bestoverlap < overlap) {
      bestoverlap = overlap;
      bestmonitor = monitors[i];
    }
  }

  return bestmonitor;
}

int main(int argc, char ** argv) {
  cxxopts::Options options("", "");
  options.add_options()("w,width", "Window width", cxxopts::value<uint32_t>())(
    "h,height",
    "Window height",
    cxxopts::value<uint32_t>());

  auto result = options.parse(argc, argv);

  // Read parameters.
  std::optional<std::pair<uint32_t, uint32_t>> cmdWindowSize;
  if (result["w"].count() != 0 && result["h"].count() != 0) {
    cmdWindowSize = std::make_pair(result["w"].as<uint32_t>(), result["h"].as<uint32_t>());
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO & io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();

  // Try to initialize GLFW.
  glfwSetErrorCallback(errorCallback);
  if (!glfwInit()) {
    return EXIT_FAILURE;
  }

  // Metal doesn't have built-in support in GLFW at the moment (Aug 2022).
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // Get screen size and scale.
  auto monitor = glfwGetPrimaryMonitor();
  auto mode = glfwGetVideoMode(monitor);

  auto [width, height] = cmdWindowSize.value_or(std::make_pair(mode->width, mode->height));

  // Create a window.
  window = glfwCreateWindow(width, height, getApp()->getName(), nullptr, nullptr);

  // Make the window to cover full screen on the current monitor and get content
  // scale.
  float xscale = 1.0f, yscale = 1.0f;
  if (auto currentMonitor = getCurrentMonitor(window); currentMonitor != monitor) {
    auto mode = glfwGetVideoMode(currentMonitor);
    int monitorX = 0, monitorY = 0;
    glfwGetMonitorPos(currentMonitor, &monitorX, &monitorY);
    if (!cmdWindowSize) {
      glfwSetWindowPos(window, monitorX, monitorY);
      glfwSetWindowSize(window, mode->width, mode->height);
    } else {
      auto const x = (mode->width - static_cast<int>(width)) / 2;
      auto const y = (mode->height - static_cast<int>(height)) / 2;
      glfwSetWindowPos(window, x, y);
    }
    glfwGetMonitorContentScale(currentMonitor, &xscale, &yscale);
  } else {
    if (cmdWindowSize) {
      auto const x = (mode->width - static_cast<int>(width)) / 2;
      auto const y = (mode->height - static_cast<int>(height)) / 2;
      glfwSetWindowPos(window, x, y);
    }
    glfwGetMonitorContentScale(monitor, &xscale, &yscale);
  }

  int fbWidth = 0, fbHeight = 0;
  glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

  // Create Metal device and layer.
  id<MTLDevice> device = MTLCreateSystemDefaultDevice();
  id<MTLCommandQueue> commandQueue = [device newCommandQueue];

  ImGui_ImplGlfw_InitForOther(window, true /* install_callbacks */);
  ImGui_ImplMetal_Init(device);

  CAMetalLayer * layer = [CAMetalLayer layer];
  layer.drawableSize = CGSize{CGFloat(fbWidth), CGFloat(fbHeight)};
  layer.device = device;
  layer.opaque = YES;
  layer.displaySyncEnabled = YES;

  // Attach Metal layer to the window.
  NSWindow * nswindow = glfwGetCocoaWindow(window);
  nswindow.contentView.layer = layer;
  nswindow.contentView.wantsLayer = YES;

  // Set up GLFW resize callback.
  glfwSetWindowSizeCallback(window, resizeCallback);

  // Try to initialize the app.
  if (!getApp()->onInitialize((__bridge MTL::Device * const)device,
                              (__bridge MTL::CommandQueue * const)commandQueue,
                              static_cast<uint32_t>(fbWidth),
                              static_cast<uint32_t>(fbHeight))) {
    return EXIT_FAILURE;
  }

  semaphore inflightFrameSemaphore(kMaxFramesInFlight);
  MTLRenderPassDescriptor * renderPassDescriptor = [MTLRenderPassDescriptor new];

  // Run main loop.
  auto lastTime = std::chrono::steady_clock::now();
  while (!glfwWindowShouldClose(window) && !needClose) {
    glfwPollEvents();

    auto const now = std::chrono::steady_clock::now();
    auto const duration = now - lastTime;
    auto const elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    // This is an important instruction.
    // Some resources are limited (e.g. drawables), we need to return used ones
    // to the pool.
    @autoreleasepool {
      glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
      layer.drawableSize = CGSize{CGFloat(fbWidth), CGFloat(fbHeight)};

      id<CAMetalDrawable> drawable = [layer nextDrawable];
      if (drawable == nil) {
        continue;
      }

      inflightFrameSemaphore.wait();

      id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
      commandBuffer.label = @"Frame Command Buffer";

      auto signalSemaphore = [&]() { inflightFrameSemaphore.signal(); };
      [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
        signalSemaphore();
      }];

      getApp()->renderFrame((__bridge MTL::CommandBuffer *)commandBuffer,
                            (__bridge MTL::Texture *)drawable.texture,
                            elapsed / 1000.0);

      [commandBuffer presentDrawable:drawable];
      [commandBuffer commit];
    }

    lastTime = now;
  }

  // Cleaning up.
  getApp()->onDeinitialize();

  ImGui_ImplMetal_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
