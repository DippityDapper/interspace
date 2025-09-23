file(GLOB COMMON_SOURCE_FILES "src/*/*")
file(GLOB COMMON_HEADER_FILES "include/common/*/*")

add_library(common STATIC
        ${COMMON_SOURCE_FILES}
        ${COMMON_HEADER_FILES}
)

target_include_directories(common PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(common PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        enet
        SQLiteCpp
)
