file(GLOB_RECURSE GAME_SOURCE_FILES "src/*")
file(GLOB_RECURSE GAME_HEADER_FILES "include/interspace/*")

add_executable(interspace
        src/main.cpp
        ${GAME_SOURCE_FILES}
        ${GAME_HEADER_FILES}
)

target_link_libraries(interspace PRIVATE
        igneous
)

target_include_directories(interspace PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_custom_target(copy_game_files ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:interspace>/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:interspace>/assets"

        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:interspace>/data"
)

add_dependencies(interspace copy_game_files)

if (WIN32)
    get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

    add_custom_command(
            TARGET interspace POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:interspace>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:interspace>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_mixer::SDL3_mixer>" "$<TARGET_FILE_DIR:interspace>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>" "$<TARGET_FILE_DIR:interspace>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLiteCpp>" "$<TARGET_FILE_DIR:interspace>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SQLite::SQLite3>" "$<TARGET_FILE_DIR:interspace>"

            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libwinpthread-1.dll" "$<TARGET_FILE_DIR:interspace>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll" "$<TARGET_FILE_DIR:interspace>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${MINGW_BIN_DIR}/libstdc++-6.dll" "$<TARGET_FILE_DIR:interspace>"
            VERBATIM
    )
endif()