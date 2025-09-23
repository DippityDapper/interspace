file(GLOB SERVER_SOURCE_FILES "src/*/*")
file(GLOB SERVER_HEADER_FILES "include/server/*/*")

add_executable(game_host
        src/main.cpp
        ${SERVER_SOURCE_FILES}
        ${SERVER_HEADER_FILES}
)

target_include_directories(game_host PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game_host PRIVATE
        common
)

set_target_properties(game_host PROPERTIES WIN32_EXECUTABLE FALSE)

if (WIN32)
    add_custom_target(copy_server_files ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game_host>/configs"
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game_host>/databases"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/configs"
            "$<TARGET_FILE_DIR:game_host>/configs"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/databases"
            "$<TARGET_FILE_DIR:game_host>/databases"
    )

    add_dependencies(game_host copy_server_files)

    add_custom_command(
            TARGET game_host POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:game_host>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:game_host>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLiteCpp>" "$<TARGET_FILE_DIR:game_host>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:sqlite3>" "$<TARGET_FILE_DIR:game_host>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>" "$<TARGET_FILE_DIR:game_host>"
            VERBATIM
    )
endif()