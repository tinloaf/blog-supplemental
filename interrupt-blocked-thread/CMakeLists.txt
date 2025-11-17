cmake_minimum_required(VERSION 3.0)
project(InterruptBlockedThreads)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(basic basic.cpp)
add_executable(jthread jthread.cpp)
add_executable(stdin stdin.cpp)
add_executable(interrupted interrupted.cpp)


