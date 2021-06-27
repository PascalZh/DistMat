include(ExternalProject)

ExternalProject_Add(
  range
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/third_party
  GIT_REPOSITORY https://github.com/klmr/cpp11-range.git
  GIT_TAG c3b7a35  # prevent git to update from remote
  GIT_SHALLOW True
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(range SOURCE_DIR)
set(RANGE_INCLUDE_DIR ${SOURCE_DIR})
message("External project range include dir: ${RANGE_INCLUDE_DIR}")

ExternalProject_Add(
  Eigen3
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/third_party
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.3.9
  GIT_SHALLOW True
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/install
)