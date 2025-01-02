cmake_minimum_required(VERSION 3.12)
project(scufftorio C)

set(CMAKE_C_STANDARD 11)

SET(ADDITIONAL_COMPILER_FLAGS "-lm -lX11")

add_definitions(-DSDL_VIDEO_DRIVER_X11=ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_COMPILER_FLAGS}")

add_executable(scuffedtorio src/main.c)

find_package(SDL2 REQUIRED)

target_include_directories(scuffedtorio PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(scuffedtorio SDL2 SDL2_image SDL2_ttf m)