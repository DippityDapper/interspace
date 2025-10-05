cmake_minimum_required(VERSION 3.30)
project(sdl3_first_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
add_subdirectory(../dapper2d ${CMAKE_BINARY_DIR}/dapper2d_build)

# --- Project modules ---
add_subdirectory(client)
