cmake_minimum_required(VERSION 3.21)

project(common)

add_definitions(-DIMGUI_IMPL_METAL_CPP)

set(SRC_LIST
  app.hpp
  glm_math.hpp
  imgui_metal.cpp
  metal_impl.cpp
  main.mm
  semaphore.hpp
  semaphore.mm
  threads.hpp
  threads.mm
  utils.hpp
)

add_library(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
  glfw
  imgui
  "-framework Metal"
  "-framework MetalKit"
  "-framework AppKit"
  "-framework Foundation"
  "-framework QuartzCore"
)

target_enable_arc(${PROJECT_NAME})
