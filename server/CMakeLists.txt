file(GLOB_RECURSE SERVER_SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE SERVER_HEADER_FILES "include/server/*.hpp")

add_executable(server
        src/main.cpp
        ${SERVER_SOURCE_FILES}
        ${SERVER_HEADER_FILES}
)

target_link_libraries(server PRIVATE
        dapper2d
)

target_include_directories(server PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (WIN32)
#    add_custom_target(copy_server_files ALL
#            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:server>/assets"
#
#            COMMAND ${CMAKE_COMMAND} -E copy_directory
#            "${CMAKE_SOURCE_DIR}/assets"
#            "$<TARGET_FILE_DIR:server>/assets"
#    )

#    add_dependencies(server copy_server_files)

    add_custom_command(
            TARGET server POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:server>"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:enet>" "$<TARGET_FILE_DIR:server>"
            VERBATIM
    )
endif()