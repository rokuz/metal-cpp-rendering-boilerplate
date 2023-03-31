cmake_minimum_required(VERSION 3.21)

project(imgui)

add_definitions(-DIMGUI_IMPL_METAL_CPP)

set(SRC_LIST
  lib/imgui_draw.cpp
  lib/imgui_tables.cpp
  lib/imgui_widgets.cpp
  lib/imgui.cpp
  lib/backends/imgui_impl_glfw.cpp
  lib/backends/imgui_impl_metal.mm
)

add_library(${PROJECT_NAME} ${SRC_LIST})

target_include_directories(${PROJECT_NAME}
  PUBLIC lib
  PUBLIC lib/backends
  PUBLIC lib/include
)

target_enable_arc(${PROJECT_NAME})
