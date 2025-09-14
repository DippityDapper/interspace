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

add_executable(game_host
        server/src/main.cpp
        server/src/Server.cpp
        server/src/Server.h
        server/src/ServerEntity.cpp
        server/src/ServerEntity.h
        common/AStar.cpp
        common/AStar.h
        common/Grid.cpp
        common/Grid.h
        common/CFGParser.cpp
        common/CFGParser.h
)

add_executable(game_client
        client/src/main.cpp
        client/src/Game.cpp
        client/src/Game.h
        client/src/ClientEntity.cpp
        client/src/ClientEntity.h
        client/src/ResourceLoader.cpp
        client/src/ResourceLoader.h
        client/src/NetworkManager.cpp
        client/src/NetworkManager.h
        common/Grid.cpp
        common/Grid.h
        client/src/Area.cpp
        client/src/Area.h
        client/src/World.cpp
        client/src/World.h
        common/CFGParser.cpp
        common/CFGParser.h
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
