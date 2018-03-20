include(CheckCXXCompilerFlag)

cmake_minimum_required(VERSION 3.5)

project(PSEmu)

# Useful for getting VS Code's Intellisense to work 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1Z)
if(COMPILER_SUPPORTS_CXX1Z)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

# We want to catch problems as soon as possible
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# GLFW
find_package(glfw3 REQUIRED)

# glad
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/3rd party/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")

add_subdirectory(src)
add_subdirectory(test)

set(SOURCE main.cpp)

add_executable(PSEmu ${SOURCE})
target_include_directories(PSEmu PRIVATE "${GLAD_DIR}/include")

target_link_libraries(PSEmu glad ${CMAKE_DL_LIBS} emu ${OPENGL_gl_LIBRARY} glfw)
