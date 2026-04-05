cmake_minimum_required(VERSION 3.15)
project(sl_codegen_lindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(../cmake/CPM.cmake)

CPMAddPackage(
    NAME Broma
    GITHUB_REPOSITORY geode-sdk/Broma
    GIT_TAG main
)

CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 12.0.0
)

CPMAddPackage(
    NAME GeodeResult
    GITHUB_REPOSITORY geode-sdk/result
    VERSION 1.3.4
)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Broma fmt::fmt GeodeResult)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/.output"
)