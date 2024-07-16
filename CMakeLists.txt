cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(
    audio
)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -pedantic
    -std=c++11
    -march=native
    -Wno-unused-parameter
    -Og
    -g
)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    src/lib.cpp
    src/player.cpp
    src/recorder.cpp
    src/rnnoise_dsp.cpp
    src/volume_dsp.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include/)

target_link_libraries(${PROJECT_NAME} portaudiocpp)
target_link_libraries(${PROJECT_NAME} portaudio)
target_link_libraries(${PROJECT_NAME} rnnoise)

add_subdirectory(examples)