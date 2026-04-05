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
    GIT_TAG 12.1.0
)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Broma fmt::fmt)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/.codegen_binaries"
)