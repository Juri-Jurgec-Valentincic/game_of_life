cmake_minimum_required(VERSION 3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(GameOfLife LANGUAGES C)
add_executable(game_of_life src/game_of_life.c)
