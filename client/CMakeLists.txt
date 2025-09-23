file(GLOB CLIENT_SOURCE_FILES "src/*/*")
file(GLOB CLIENT_HEADER_FILES "include/client/*/*")

add_executable(game_client
        src/main.cpp
        ${CLIENT_SOURCE_FILES}
        ${CLIENT_HEADER_FILES}
)

target_include_directories(game_client PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game_client PRIVATE
        common
)

if (WIN32)
    add_custom_target(copy_client_files ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game_client>/assets"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game_client>/configs"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:game_client>/assets"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/configs"
            "$<TARGET_FILE_DIR:game_client>/configs"
    )

    add_dependencies(game_client copy_client_files)

    add_custom_command(
            TARGET game_client POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:game_client>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:game_client>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>" "$<TARGET_FILE_DIR:game_client>"
            VERBATIM
    )
endif()