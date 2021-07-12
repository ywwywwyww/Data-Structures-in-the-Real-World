cmake_minimum_required(VERSION 3.15)
project(pa2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(pa2
        main.cc
        Node.cc
        Node.h
        Vector.h Vector.cc)
