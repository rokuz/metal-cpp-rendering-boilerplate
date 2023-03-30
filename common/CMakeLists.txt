cmake_minimum_required(VERSION 3.21)

project(common)

set(SRC_LIST
  app.hpp
  glm_math.hpp
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
  "-framework Metal"
  "-framework MetalKit"
  "-framework AppKit"
  "-framework Foundation"
  "-framework QuartzCore"
)

target_enable_arc(${PROJECT_NAME})
