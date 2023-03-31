## Metal-cpp Rendering Boilerplate

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

# How to use:
1. Add `metal-cpp-rendering-boilerplate` as submodule. E.g. `git submodule add https://github.com/rokuz/metal-cpp-rendering-boilerplate 3party/metal-cpp-rendering-boilerplate`
2. `git submodule update --init --recursive`
3. Use the following template in your CMake
```
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

# Optional: Copy Metal libraries to the bundle if you have some. Use target_add_msl_library(${PROJECT_NAME} ${SRC_LIST_METAL}) to create one.
target_bundle_msl_libraries(${PROJECT_NAME} <SPECIFY TARGET's NAME>)

# Enable ARC for a target.
target_enable_arc(${PROJECT_NAME})
```
