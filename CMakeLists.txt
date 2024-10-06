cmake_minimum_required(VERSION 3.29)

project(codingame-fall-2024 LANGUAGES CXX)

set(SOURCE_DIR src)
set(SOURCE_LIST main.cpp)
list(TRANSFORM SOURCE_LIST PREPEND "${SOURCE_DIR}/")

add_executable(app "${SOURCE_LIST}")
