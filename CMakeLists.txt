cmake_minimum_required(VERSION 3.30)
project(interspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
add_subdirectory(../igneous ${CMAKE_BINARY_DIR}/igneous_build)

# --- Project modules ---
add_subdirectory(interspace)
