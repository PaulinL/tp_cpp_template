cmake_minimum_required(VERSION 3.15)
project(tp5_clion)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(tp5_v1 mainV1.cpp)
add_executable(tp5_v2 mainV2.cpp)