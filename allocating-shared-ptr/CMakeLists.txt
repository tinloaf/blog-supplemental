cmake_minimum_required(VERSION 3.0)
project(Poolbench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Boost
find_package(Boost REQUIRED)

# Add executable
add_executable(benchmark benchmark.cpp)

# Include Boost directories
target_include_directories(benchmark PRIVATE ${Boost_INCLUDE_DIRS})

# Link Boost libraries
target_link_libraries(benchmark PRIVATE ${Boost_LIBRARIES})
