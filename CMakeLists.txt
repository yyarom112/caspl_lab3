cmake_minimum_required(VERSION 3.8)
project(lab3)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main[1].c toy_printf.c toy_stdio.h)
add_executable(lab3 ${SOURCE_FILES})