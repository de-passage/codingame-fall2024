cmake_minimum_required(VERSION 3.29)

project(codingame-fall-2024 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(strong-types 1.0.0 REQUIRED EXACT)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCE_LIST main.cpp math.cpp)
list(TRANSFORM SOURCE_LIST PREPEND "${SOURCE_DIR}/")

add_executable(app "${SOURCE_LIST}")
target_link_libraries(app PRIVATE strong-types::strong-types)
target_include_directories(app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

get_target_property(app_SOURCES app SOURCES)
get_target_property(app_INCLUDE_DIRECTORIES app INCLUDE_DIRECTORIES)
get_target_property(strong-types_INCLUDE_DIRECTORIES strong-types::strong-types INTERFACE_INCLUDE_DIRECTORIES)

list(TRANSFORM app_INCLUDE_DIRECTORIES PREPEND "-I")
list(TRANSFORM strong-types_INCLUDE_DIRECTORIES PREPEND "-I")

set(generated_file "${CMAKE_CURRENT_BINARY_DIR}/result.cpp")
set(generation_dependencies
  types.hpp
  actions.hpp
  common.hpp
  math.hpp
)
list(TRANSFORM generation_dependencies PREPEND "${SOURCE_DIR}/")

add_custom_command(
  OUTPUT "${generated_file}"
  DEPENDS ${app_SOURCES} "${generation_dependencies}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND
    "${CMAKE_CURRENT_SOURCE_DIR}/scripts/combine.bash"
    ${app_INCLUDE_DIRECTORIES}
    ${strong-types_INCLUDE_DIRECTORIES}
    ${app_SOURCES}
    -o "${generated_file}"
  COMMAND
    ${CMAKE_COMMAND} -E echo "-- Copy the generate file to clipboard:"
  COMMAND
    ${CMAKE_COMMAND} -E echo "  xclip -i ${generated_file} -selection clipboard"
  )

add_custom_target(
  generate
  DEPENDS "${generated_file}"
  )
