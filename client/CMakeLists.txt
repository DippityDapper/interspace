file(GLOB_RECURSE CLIENT_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE CLIENT_HEADER_FILES "include/client/*.hpp")

add_executable(client
        src/main.cpp
        ${CLIENT_SOURCE_FILES}
        ${CLIENT_HEADER_FILES}
)

target_link_libraries(client PRIVATE
        dapper2d
)

target_include_directories(client PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (WIN32)
    add_custom_target(copy_client_files ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:client>/assets"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:client>/assets"
    )

    add_dependencies(client copy_client_files)

    add_custom_command(
            TARGET client POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:client>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_image::SDL3_image>" "$<TARGET_FILE_DIR:client>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>" "$<TARGET_FILE_DIR:client>"
            VERBATIM
    )
endif()