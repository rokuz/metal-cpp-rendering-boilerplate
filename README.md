# Metal-cpp Rendering Boilerplate

It contains:
- MacOS window (based on [GLFW library](https://github.com/glfw/glfw));
- UI integration powered by [Dear ImGui](https://github.com/ocornut/imgui);
- cpp wrappers for OS-level components (semaphores, dispatch);
- metal-cpp initialization and lifecycle management;
- cmake helpers and utilities;
- boilerplate dependencies ([GLM](https://github.com/g-truc/glm)).

Compatibility:
- MacOS 13+
- C++ 20

## How to use:
1. Add `metal-cpp-rendering-boilerplate` as submodule. E.g. `git submodule add https://github.com/rokuz/metal-cpp-rendering-boilerplate 3party/metal-cpp-rendering-boilerplate`
2. `git submodule update --init --recursive`
3. Use the following template in your CMake
```cmake
cmake_minimum_required(VERSION 3.21)

project(<SPECIFY PROJECT'S NAME>)

include("./3party/metal-cpp-rendering-boilerplate/dependencies.cmake")

include_metal_cpp_rendering_boilerplate("./3party/metal-cpp-rendering-boilerplate")

<SPECIFY YOUR SOURCES AND OTHER STUFF HERE>

# Optional: set up default bundle icon
set_source_files_properties(${BUNDLE_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${BUNDLE_ICON} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
  <SPECIFY YOUR LIBRARIES TO LINK>
  common
)

# Optional: Copy Metal libraries to the bundle if you have some. 
# Use target_add_msl_library(${PROJECT_NAME} ${SRC_LIST_METAL}) to create one.
target_bundle_msl_libraries(${PROJECT_NAME} <SPECIFY TARGET's NAME>)

# Enable ARC for a target.
target_enable_arc(${PROJECT_NAME})
```
4. Create a class derived from `App` class. The following methods must be defined.
```cpp
  [[nodiscard]] virtual bool onInitialize(MTL::Device * const device,
                                          MTL::CommandQueue * const commandQueue,
                                          uint32_t screenWidth,
                                          uint32_t screenHeight);
```
The following methods may be defined.
```cpp
  [[nodiscard]] virtual char const * const getName() const;
  
  virtual void onDeinitialize();

  virtual void renderFrame(MTL::CommandBuffer * frameCommandBuffer,
                           MTL::Texture * outputTexture,
                           double elapsedSeconds);

  virtual void onResize(uint32_t screenWidth, uint32_t screenHeight);
```
5. In a cpp-file, `getApp()` function must be implemented. It must create the `App` class's successor instance.
```cpp
App * getApp() {
  static DerivedClass app;
  return &app;
}
```

> Tip: Use template files [`template.hpp`](https://github.com/rokuz/metal-cpp-rendering-boilerplate/blob/main/template.hpp) and [`template.cpp`](https://github.com/rokuz/metal-cpp-rendering-boilerplate/blob/main/template.cpp) for the quick start.
<img width="752" alt="Demo" src="https://user-images.githubusercontent.com/5437220/229228859-5883042e-29d2-4bc1-abf3-cba4d4e88bd2.png">
