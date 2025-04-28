cmake_minimum_required(VERSION 4.0)
project(game_boy_emulator C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE sources src/*.c)
add_library(gbe_lib ${sources})

add_executable(gbe src/main.c)
target_link_libraries(gbe PRIVATE gbe_lib)

add_executable(tests
    test/test.c
)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_library(Unity
	external/Unity/src/unity.c
)
target_include_directories(Unity PUBLIC
	external/Unity/src
)

target_link_libraries(tests PRIVATE gbe_lib Unity)

enable_testing()
add_test(NAME tests COMMAND tests)
