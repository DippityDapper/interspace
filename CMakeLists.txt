cmake_minimum_required(VERSION 3.30)
project(interspace)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#option(ENABLE_ASAN "Enable AddressSanitizer" ON)
#
#if (ENABLE_ASAN)
#    add_compile_options(
#            -fsanitize=address
#            -fno-omit-frame-pointer
#            -g
#            -O1
#    )
#    add_link_options(-fsanitize=address)
#endif()


# --- Dependencies ---
add_subdirectory(../igneous ${CMAKE_BINARY_DIR}/igneous_build)

# --- Project modules ---
add_subdirectory(interspace)
