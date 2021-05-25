cmake_minimum_required(VERSION 3.16)
project(DistMat)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
set(CMAKE_CXX_STANDARD 20)

include_directories(DistMat/)

add_executable(test_matrix.out test/test_matrix.cpp test/bench/bench.cpp)

enable_testing()
add_test(NAME test_matrix COMMAND test_matrix.out)