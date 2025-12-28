file(GLOB_RECURSE GAME_SOURCE_FILES "src/*/*")
file(GLOB_RECURSE GAME_HEADER_FILES "include/game/*/*")

add_executable(game
        src/main.cpp
        ${GAME_SOURCE_FILES}
        ${GAME_HEADER_FILES}
)

target_link_libraries(game PRIVATE
        igneous
)

target_include_directories(game PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (WIN32)
    add_custom_target(copy_game_files ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:game>/assets"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:game>/assets"
    )

    add_dependencies(game copy_game_files)

    add_custom_command(
            TARGET game POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:game>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:game>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_mixer::SDL3_mixer>" "$<TARGET_FILE_DIR:game>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>" "$<TARGET_FILE_DIR:game>"
            VERBATIM
    )
endif()