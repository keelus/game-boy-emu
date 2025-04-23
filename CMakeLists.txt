cmake_minimum_required(VERSION 4.0)
project(game_boy_emulator C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE sources src/*.c)
add_executable(gbe ${sources})
