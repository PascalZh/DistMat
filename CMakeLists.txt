cmake_minimum_required(VERSION 3.16)
project(DistMat)

# global settings
set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Werror)

# aliasing for executables
set(TEST_MATRIX test_matrix.out)

add_library(bench STATIC test/bench/bench.cpp)
target_include_directories(bench PUBLIC DistMat/)

add_executable(${TEST_MATRIX} test/test_matrix.cpp)
target_link_libraries(${TEST_MATRIX} bench)

enable_testing()
add_test(NAME test_matrix COMMAND test_matrix.out)