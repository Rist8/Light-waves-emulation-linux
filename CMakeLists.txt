cmake_minimum_required(VERSION 3.7)

project(SDL2)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(SDL2 SDL.cpp)
target_link_libraries(SDL2 ${SDL2_LIBRARIES})