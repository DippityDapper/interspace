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

igneous_copy_runtime_dependencies(interspace "${CMAKE_SOURCE_DIR}")