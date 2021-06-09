cmake_minimum_required(VERSION 3.16)
# global settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Werror)

project(DistMat)

# aliasing for executables
set(BIN_TEST_MATRIX test_matrix.out)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories (${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

add_library(BasicTest test/Bench.cpp)
target_link_libraries(BasicTest PUBLIC ${PROJECT_NAME})

add_executable(${BIN_TEST_MATRIX} test/test_matrix.cpp)
target_link_libraries(${BIN_TEST_MATRIX} BasicTest)

enable_testing()
add_test(NAME test_matrix COMMAND ${BIN_TEST_MATRIX})