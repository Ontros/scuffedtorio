cmake_minimum_required(VERSION 3.12)
project(sdlapp C)

set(CMAKE_C_STANDARD 11)

add_executable(scuffedtorio main.c)

find_package(SDL2 REQUIRED)

target_include_directories(scuffedtorio PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(scuffedtorio SDL2 SDL2_image SDL2_ttf)