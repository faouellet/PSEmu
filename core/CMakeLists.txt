cmake_minimum_required(VERSION 3.5)

file(GLOB CPU_SOURCES	"./cpu/*")
file(GLOB DEBUG_SOURCES	"./debug/*")
file(GLOB MEM_SOURCES	"./memory/*")
file(GLOB GPU_SOURCES	"./video/*")
file(GLOB SPU_SOURCES	"./sound/*")
file(GLOB UTILS_SOURCES	"./utils/*")

SOURCE_GROUP(emu\\cpu    FILES ${CPU_SOURCES})
SOURCE_GROUP(emu\\debug  FILES ${DEBUG_SOURCES})
SOURCE_GROUP(emu\\memory FILES ${MEM_SOURCES})
SOURCE_GROUP(emu\\video  FILES ${GPU_SOURCES})
SOURCE_GROUP(emu\\sound  FILES ${SPU_SOURCES})
SOURCE_GROUP(emu\\utils  FILES ${UTILS_SOURCES})

add_library( emu STATIC
        ${CPU_SOURCES}
        ${DEBUG_SOURCES}
        ${MEM_SOURCES}
        ${GPU_SOURCES}
        ${SPU_SOURCES}
        ${UTILS_SOURCES}
        )

target_include_directories(emu PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
        