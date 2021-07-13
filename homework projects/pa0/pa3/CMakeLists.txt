cmake_minimum_required(VERSION 3.15)
project(pa3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(pa3
        CustomClass.h
        main.cpp
        Object.h)
