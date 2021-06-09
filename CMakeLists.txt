cmake_minimum_required(VERSION 3.16)
# global settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Werror)

project(DistMat)

# aliasing for executables
set(TEST_MATRIX test_matrix.out)

add_library(DistMat INTERFACE)
target_include_directories(DistMat INTERFACE .)
target_compile_features(DistMat INTERFACE cxx_std_20)

add_library(bench STATIC test/bench/bench.cpp)
target_link_libraries(bench PUBLIC DistMat)

add_executable(${TEST_MATRIX} test/test_matrix.cpp)
target_link_libraries(${TEST_MATRIX} bench)

enable_testing()
add_test(NAME test_matrix COMMAND test_matrix.out)