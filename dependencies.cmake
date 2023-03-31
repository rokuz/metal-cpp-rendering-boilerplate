# Copyright © 2022 Roman Kuznetsov.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(PLATFORM_MAC TRUE)
  message("Setting PLATFORM_MAC to true")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
  set(PLATFORM_IOS TRUE)
  message("Setting PLATFORM_IOS to true")
endif()

# Create a target for Metal shaders and links it with C++ target library.
function(target_add_msl_library target sources)
  set(_SOURCES ${sources} ${ARGN})

  foreach(source IN LISTS _SOURCES)
    if(source MATCHES \\.\(metal\)$)
      list(APPEND _SOURCES_METAL ${source})
      set_source_files_properties(${source} PROPERTIES LANGUAGE METAL)
    endif()
  endforeach()

  if (${PLATFORM_IOS})
    set(_METAL_SHADERS_TARGET "iphoneos")
  elseif (${PLATFORM_MAC})
    set(_METAL_SHADERS_TARGET "macosx")
  else()
    message(FATAL_ERROR "Unsupported platform")
  endif()
  set(_METAL_SHADERS_IR "${target}.air")
  set(_METAL_SHADERS_LIBRARY "${target}.metallib")

  add_custom_command(
    TARGET
      ${target}
    WORKING_DIRECTORY
      "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS
      _SOURCES
    COMMAND
      xcrun -sdk ${_METAL_SHADERS_TARGET} metal -c ${_SOURCES_METAL} -o "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${_METAL_SHADERS_IR}"
    COMMENT "Compiling ${target} shaders ..."
  )

  add_custom_command(
    TARGET
      ${target}
    WORKING_DIRECTORY
      "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/${_METAL_SHADERS_IR}
    COMMAND
      xcrun -sdk ${_METAL_SHADERS_TARGET} metallib "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${_METAL_SHADERS_IR}" -o "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${_METAL_SHADERS_LIBRARY}"
    COMMENT "Linking ${target} shaders ..."
  )
endfunction()

# Copy MSL libraries to a bundle.
function(target_bundle_msl_libraries target target_metal_libraries)
  set(_TARGET_METAL_LIBRARIES ${target_metal_libraries} ${ARGN})
  set(_BUNDLE_FOLDER "${CMAKE_BINARY_DIR}/$<CONFIG>/${PROJECT_NAME}.app")
  set(_RESOURCES_FOLDER "${_BUNDLE_FOLDER}/Contents/Resources")
  foreach(target_metal_library IN LISTS _TARGET_METAL_LIBRARIES)
    add_custom_command(
      TARGET
        ${target}
      WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"
      DEPENDS
        "$<TARGET_FILE_DIR:${target_metal_library}>/${target_metal_library}.metallib"
      COMMAND
        mkdir -p ${_RESOURCES_FOLDER} && cp -r "$<TARGET_FILE_DIR:${target_metal_library}>/${target_metal_library}.metallib" "${_RESOURCES_FOLDER}/${target_metal_library}.metallib"
      COMMENT "Copy ${target_metal_library}.metallib into bundle ..."
    )
    add_dependencies(${target} "${target_metal_library}")
  endforeach()
endfunction()

# Enable ARC for a target.
function(target_enable_arc target)
  set_property(TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
endfunction()

# Include metal-cpp rendering boilerplate to the project.
function(include_metal_cpp_rendering_boilerplate path_to_boilerplate)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
  set(GLFW_INSTALL OFF CACHE BOOL "")

  include_directories(${path_to_boilerplate})
  include_directories(${PROJECT_BINARY_DIR})
  include_directories(${path_to_boilerplate}/3party/cxxopts/include)
  include_directories(${path_to_boilerplate}/3party/imgui/lib)
  include_directories(${path_to_boilerplate}/3party/glfw/include)
  include_directories(${path_to_boilerplate}/3party/glm)
  include_directories(${path_to_boilerplate}/3party/metal-cpp)

  add_subdirectory(${path_to_boilerplate}/3party/glfw)
  add_subdirectory(${path_to_boilerplate}/3party/glm)
  add_subdirectory(${path_to_boilerplate}/3party/imgui)

  add_subdirectory(${path_to_boilerplate}/common)

  add_definitions(-DIMGUI_IMPL_METAL_CPP)

  configure_file(
    "${path_to_boilerplate}/boilerplate_config.hpp.in"
    "${PROJECT_BINARY_DIR}/boilerplate_config.hpp"
  )

  set(MACOSX_BUNDLE_ICON_FILE rokuz.icns PARENT_SCOPE)
  set(BUNDLE_ICON ${path_to_boilerplate}/assets/rokuz.icns PARENT_SCOPE)
endfunction()
