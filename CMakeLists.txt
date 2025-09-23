cmake_minimum_required(VERSION 3.30)
project(sdl3_first_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
add_subdirectory(libs/SDL3)
add_subdirectory(libs/SDL3_image)
add_subdirectory(libs/enet)
add_subdirectory(libs/SQLiteCpp)
add_subdirectory(libs/imgui)

# --- Project modules ---
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
