cmake_minimum_required(VERSION 3.30)
project(sdl3_first_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
add_subdirectory(libs/SDL3)
add_subdirectory(libs/SDL3_image)
add_subdirectory(libs/enet)
add_subdirectory(libs/SQLiteCpp)

set(IMGUI_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/imgui_demo.cpp  # optional, good for testing
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_sdl3.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

# --- Project modules ---
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
