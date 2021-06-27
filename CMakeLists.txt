cmake_minimum_required(VERSION 3.16)
# global settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CXX_FLAGS -fdump-tree-original-raw all)
add_compile_options(-Wall -Werror)

project(DistMat)

# aliasing for executables
set(BIN_TEST_MATRIX test_matrix.out)

option(DISTMAT_InstallExternalProject "Install external projects" OFF)

set(RANGE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/src/range)
if(DISTMAT_InstallExternalProject)
  include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallExternalProject.cmake")
endif()

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# DistMat library declaration
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories (${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
  ${RANGE_INCLUDE_DIR}
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

add_library(BasicBench test/Bench.cpp)
target_link_libraries(BasicBench PUBLIC ${PROJECT_NAME})

add_executable(${BIN_TEST_MATRIX} test/test_matrix.cpp)
target_link_libraries(${BIN_TEST_MATRIX} BasicBench)

add_executable(experiment_expr.out test/experiment_expr.cpp)
target_compile_features(experiment_expr.out PRIVATE cxx_std_20)
target_link_libraries(experiment_expr.out PRIVATE Eigen3::Eigen)

enable_testing()
add_test(NAME test_matrix COMMAND ${BIN_TEST_MATRIX})