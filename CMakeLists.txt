include(CheckCXXCompilerFlag)

cmake_minimum_required(VERSION 3.5)

project(PSEmu)

# Useful for getting VS Code's Intellisense to work 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1Z)
if(COMPILER_SUPPORTS_CXX1Z)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

# We want to catch problems as soon as possible
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

# OpenGL
find_package(OpenGL REQUIRED)

# ImGui
set(IMGUI_DIR "../3rd party/imgui")
include_directories(${IMGUI_DIR})

add_subdirectory(src)
add_subdirectory(test)

set(SOURCE main.cpp)

add_executable(PSEmu ${SOURCE} ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp)
target_link_libraries(PSEmu emu)
