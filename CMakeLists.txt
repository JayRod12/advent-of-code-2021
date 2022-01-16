
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

project(aoc)

add_executable(1 1/solution.cpp)
add_executable(2 2/solution.cpp)
add_executable(3 3/solution.cpp)
add_executable(4 4/solution.cpp)
add_executable(5 5/solution.cpp)
add_executable(6 6/solution.cpp)
add_executable(7 7/solution.cpp)
add_executable(8 8/solution.cpp)
add_executable(9 9/solution.cpp)
add_executable(10 10/solution.cpp)
add_executable(11 11/solution.cpp)
add_executable(12 12/solution.cpp)
add_executable(13 13/solution.cpp)
add_executable(14 14/solution.cpp)
add_executable(15 15/solution.cpp)
add_executable(16 16/solution.cpp)
add_executable(17 17/solution.cpp)
add_executable(18 18/solution.cpp)
add_executable(19 19/solution.cpp)
target_link_libraries(19 Eigen3::Eigen)


