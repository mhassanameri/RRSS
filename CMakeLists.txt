cmake_minimum_required(VERSION 4.1)
project(RRSS)

set(CMAKE_CXX_STANDARD 20)

add_executable(RRSS main.cpp
        RandomRobustSS.cpp
        RandomRobustSS.h)
