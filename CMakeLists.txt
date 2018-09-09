cmake_minimum_required(VERSION 3.11)
project(saitama C)

set(CMAKE_C_FLAGS "-Wall -Wextra -O3 -msse3 -mpopcnt -lm")
set(CMAKE_C_STANDARD 99)

include_directories(src)

file(GLOB SOURCES "src/*.c")

add_executable(saitama ${SOURCES})
