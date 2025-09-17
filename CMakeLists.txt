cmake_minimum_required(VERSION 3.30)
project(sdl3_first_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

function(include_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG ${gitTag}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(${libName})
endfunction()

# SDL3
find_package(SDL3 QUIET)
if (NOT SDL3_FOUND)
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.2.20)
endif ()

# SDL_image
find_package(SDL3_image QUIET)
if (NOT SDL3_image_FOUND)
    include_dependency(SDL3_image https://github.com/libsdl-org/SDL_image.git release-3.2.4)
    set(SDL3IMAGE_AVIF OFF CACHE BOOL "" FORCE)
    set(SDL3IMAGE_VENDORED OFF CACHE BOOL "" FORCE)
endif ()

# ENet
find_package(ENet QUIET)
if (NOT ENet_FOUND)
    include_dependency(enet https://github.com/lsalzman/enet.git v1.3.18)

    file(GLOB ENET_SRC
            ${enet_SOURCE_DIR}/*.c
            ${enet_SOURCE_DIR}/*.h
    )

    add_library(enet_lib STATIC ${ENET_SRC})
    target_include_directories(enet_lib PUBLIC ${enet_SOURCE_DIR}/include)

    if (WIN32)
        target_link_libraries(enet_lib PUBLIC ws2_32 winmm)
    endif ()

    add_library(ENet::ENet ALIAS enet_lib)
endif ()

set(COMMON_SOURCE_FILES "")
file(GLOB COMMON_SOURCE_FILES "common/src/*")

set(SERVER_SOURCE_FILES "")
file(GLOB SERVER_SOURCE_FILES "server/src/*")
add_executable(game_host
        ${SERVER_SOURCE_FILES}
        ${COMMON_SOURCE_FILES}
)

set(CLIENT_SOURCE_FILES "")
file(GLOB CLIENT_SOURCE_FILES "client/src/*")
add_executable(game_client
        ${CLIENT_SOURCE_FILES}
        ${COMMON_SOURCE_FILES}
)

set_target_properties(game_host PROPERTIES WIN32_EXECUTABLE FALSE)

target_link_libraries(game_host PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        ENet::ENet
)

target_link_libraries(game_client PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        ENet::ENet
)

if (WIN32)
    add_custom_command(
            TARGET game_client POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:game_client>/assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/configs"
            "$<TARGET_FILE_DIR:game_client>/configs"
    )

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
            VERBATIM
    )
endif ()
