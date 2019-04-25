cmake_minimum_required(VERSION 3.13)
project(Euo3 C)

set(CMAKE_C_STANDARD 99)

add_executable(Euo3 map.c euo.c map.h euo.h list.h)