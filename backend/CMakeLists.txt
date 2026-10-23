cmake_minimum_required(VERSION 3.20)

project(ForgeHTTP
    VERSION 0.1.0
    DESCRIPTION "A lightweight HTTP/1.1 server and application framework"
    LANGUAGES CXX
)

include(CTest)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------
# Dependencies
# --------------------------------------------------

include(FetchContent)
find_package(SQLite3 REQUIRED)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(json)

# --------------------------------------------------
# ForgeHTTP source files
# --------------------------------------------------

file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

add_executable(forge
    ${SOURCES}
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

target_link_libraries(forge PRIVATE
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)

target_include_directories(forge PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(forge_tests
    ${SOURCES}
    "${PROJECT_SOURCE_DIR}/tests/test.cpp"
)

add_executable(forge_integration_tests
    ${SOURCES}
    "${PROJECT_SOURCE_DIR}/tests/integration.cpp"
)
target_link_libraries(forge_integration_tests PRIVATE
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)
target_include_directories(forge_integration_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
if (MSVC)
    target_compile_options(forge_integration_tests PRIVATE /W4)
else()
    target_compile_options(forge_integration_tests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

if (BUILD_TESTING)
    add_test(NAME forge_unit_tests COMMAND forge_tests)
    add_test(NAME forge_integration_tests COMMAND forge_integration_tests)
endif()

target_link_libraries(forge_tests PRIVATE
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
)

target_include_directories(forge_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------

if (MSVC)
    target_compile_options(forge PRIVATE /W4)

else()
    target_compile_options(forge PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()

# --------------------------------------------------
# Compiler Warnings
# --------------------------------------------------
if (MSVC)
    target_compile_options(forge_tests PRIVATE /W4)

else()
    target_compile_options(forge_tests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()