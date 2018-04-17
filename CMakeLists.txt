cmake_minimum_required(VERSION 3.5)
project(PSEmu LANGUAGES CXX)

option(ENABLE_TESTING "Enable testing" ON)

# Useful for getting VS Code's Intellisense to work 
set(PSEmu CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We want to catch problems as soon as possible
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

add_subdirectory(core)
add_subdirectory(ui)
if(ENABLE_TESTING)
    #add_subdirectory(test)
endif()

set(SOURCE main.cpp)

add_executable(PSEmu ${SOURCE})
target_link_libraries(PSEmu emu ui)
