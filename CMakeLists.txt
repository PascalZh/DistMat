cmake_minimum_required(VERSION 3.3)
project(DistMat)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
add_compile_options(-std=c++20)

include_directories(DistMat/)

add_executable(test_matrix.out test/test_matrix.cpp)

enable_testing()
add_test(NAME test_matrix COMMAND test_matrix.out)