cmake_minimum_required(VERSION 3.20) #CMake version check

project(ForgeHTTP
    VERSION 0.1.0
    DESCRIPTION "A lightweight HTTP/1.1 server and application framework"
    LANGUAGES CXX
)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# main executable
add_executable(forge ${SOURCES})

# Project Headers
target_include_directories(forge
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Compiler Warnings
if (MSVC)
    target_compile_options(forge PRIVATE /W4)
else()
    target_compile_options(forge PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        # # Sanitizers
        # -fsanitize=address,undefined
        # -fno-omit-frame-pointer
    )

    # target_link_options(forge PRIVATE
    #     -fsanitize=address,undefined
    # )
endif()