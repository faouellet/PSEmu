cmake_minimum_required(VERSION 3.5)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(PSEmuTests test.cpp)
target_link_libraries(PSEmuTests ${GTEST_LIBRARIES} pthread)