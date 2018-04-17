cmake_minimum_required(VERSION 3.5)
project(EmuUI LANGUAGES CXX)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTORCC ON)

add_library(ui mainwindow.cpp debugwindow.cpp renderwidget.cpp resources.qrc)

target_include_directories(ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(ui Qt5::Core Qt5::Widgets)