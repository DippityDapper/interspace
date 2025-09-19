cmake_minimum_required(VERSION 3.30)
project(sdl3_first_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SDL3 ---
add_subdirectory(libs/SDL3)

# --- SDL_image ---
set(SDL3IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL3IMAGE_VENDORED OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/SDL3_image)

# --- ENet ---
file(GLOB ENET_SRC
        libs/enet/*.c
        libs/enet/*.h
)
add_library(enet_lib STATIC ${ENET_SRC})
target_include_directories(enet_lib PUBLIC libs/enet/include)
if (WIN32)
    target_link_libraries(enet_lib PUBLIC ws2_32 winmm)
endif ()
add_library(ENet::ENet ALIAS enet_lib)

# --- SQLiteCpp ---
add_subdirectory(libs/SQLiteCpp)

# --- Sources ---
file(GLOB COMMON_SOURCE_FILES "common/src/*")
file(GLOB SERVER_SOURCE_FILES "server/src/*")
file(GLOB CLIENT_SOURCE_FILES "client/src/*")

add_executable(game_host
        ${SERVER_SOURCE_FILES}
        ${COMMON_SOURCE_FILES}
)
add_executable(game_client
        ${CLIENT_SOURCE_FILES}
        ${COMMON_SOURCE_FILES}
)

set_target_properties(game_host PROPERTIES WIN32_EXECUTABLE FALSE)

target_link_libraries(game_host PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        ENet::ENet
        SQLiteCpp
)

target_link_libraries(game_client PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        ENet::ENet
        SQLiteCpp
)

# --- Windows-specific asset copying ---
if (WIN32)
    file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")
    file(COPY "${CMAKE_SOURCE_DIR}/configs" DESTINATION "${CMAKE_BINARY_DIR}")

    add_custom_target(copy_assets ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game_client>/assets"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game_client>/configs"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game_host>/databases"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:game_client>/assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/configs"
            "$<TARGET_FILE_DIR:game_client>/configs"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/databases"
            "$<TARGET_FILE_DIR:game_host>/databases"
    )

    add_dependencies(game_client copy_assets)
    add_dependencies(game_host copy_assets)

    add_custom_command(
            TARGET game_client POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:game_client>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:game_client>"
            VERBATIM
    )
    add_custom_command(
            TARGET game_host POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:game_host>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:game_host>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLiteCpp>" "$<TARGET_FILE_DIR:game_host>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:sqlite3>" "$<TARGET_FILE_DIR:game_host>"
            VERBATIM
    )
endif ()
