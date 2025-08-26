cmake_minimum_required(VERSION 3.31)
project(ByteRacers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

find_package(SDL3 REQUIRED)
include_directories(ByteRacers ${SDL3_INCLUDE_DIR})

add_executable(ByteRacers main.cpp)
target_link_libraries(ByteRacers ${SDL3_LIBRARY})