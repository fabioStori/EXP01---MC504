cmake_minimum_required(VERSION 3.15)
project(BatManager C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(BatManager main.c car.c car.h directions.h directions.c)