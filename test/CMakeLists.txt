cmake_minimum_required(VERSION 3.5)

INCLUDE(ExternalProject)

ExternalProject_Add(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

enable_testing()
add_executable(PSEmuTests cputests.cpp cpufixture.cpp)
target_link_libraries(PSEmuTests emu gtest gtest_main pthread)