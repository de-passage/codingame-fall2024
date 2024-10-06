cmake_minimum_required(VERSION 3.29)

project(codingame-fall-2024 LANGUAGES CXX)

find_package(strong-types 1.0.0 REQUIRED EXACT)

set(SOURCE_DIR src)
set(SOURCE_LIST main.cpp)
list(TRANSFORM SOURCE_LIST PREPEND "${SOURCE_DIR}/")

add_executable(app "${SOURCE_LIST}")
target_link_libraries(app PRIVATE strong-types::strong-types)
