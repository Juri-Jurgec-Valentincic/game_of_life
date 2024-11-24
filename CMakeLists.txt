cmake_minimum_required(VERSION 3.0)
project(GameOfLife LANGUAGES C)
set(CMAKE_C_FLAGS_DEBUG "-ggdb3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)
add_executable(game_of_life src/game_of_life.c)
