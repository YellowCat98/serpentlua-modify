cmake_minimum_required(VERSION 3.22)
project(yellowcat98_modify LANGUAGES C CXX RC)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)
include(ExternalProject)

set(CMAKE_RC_COMPILER llvm-rc)

set(CODEGEN_BINARY "${CMAKE_SOURCE_DIR}/codegen/.codegen_binaries/sl_codegen_lindings.exe")
set(GENERATED_FOLDER "${CMAKE_SOURCE_DIR}/__generated_lindings")

file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/__generated_lindings")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__generated_lindings")

execute_process(
    COMMAND ${CMAKE_COMMAND} -B ${CMAKE_BINARY_DIR}/__codegen_build -G "Ninja"
        -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
        -DCMAKE_BUILD_TYPE=Release
        -S ${CMAKE_SOURCE_DIR}/codegen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE _cfg
)
if(NOT _cfg EQUAL 0)
    message(FATAL_ERROR "[modify] Codegen configure failed")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/__codegen_build --config Release
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE _build
)
if(NOT _build EQUAL 0)
    message(FATAL_ERROR "[modify] Codegen build failed")
endif()


execute_process(
    COMMAND "${CODEGEN_BINARY}" "${CMAKE_SOURCE_DIR}/lindings.bro" "${GENERATED_FOLDER}"
    RESULT_VARIABLE _gen
)

if(NOT _gen EQUAL 0)
    message(FATAL_ERROR "[modify] Codegen execution failed")
endif()

include(${CMAKE_SOURCE_DIR}/__generated_lindings/__lindings_generated.cmake)


set(SOURCES
    src/main.cpp
    src/utils.cpp
)

add_library(yellowcat98_modify SHARED ${SOURCES} ${__GENERATED_LINDINGS_SOURCES})

set_target_properties(yellowcat98_modify PROPERTIES
    OUTPUT_NAME "yellowcat98.modify"
    SUFFIX ".slp"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/.output"
)

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    VERSION 3.5.0 # wonder who thought it was a good idea to not make the actual latest version the latest version!
)

target_include_directories(${PROJECT_NAME} PRIVATE ${sol2_SOURCE_DIR}/include)

target_link_libraries(yellowcat98_modify PUBLIC
    "${CMAKE_SOURCE_DIR}/lib/lua.lib"
    GeodeResult
)

set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/serpentlua.rc")
set(RESOURCE_OUTPUT "${CMAKE_BINARY_DIR}/serpentlua.res")

add_custom_command(
    OUTPUT ${RESOURCE_OUTPUT}
    COMMAND ${CMAKE_RC_COMPILER} /fo ${RESOURCE_OUTPUT} ${RESOURCE_FILE}
    DEPENDS ${RESOURCE_FILE}
    COMMENT "Compiling resource file serpentlua.rc with llvm-rc"
)

add_custom_target(serpentlua_res DEPENDS ${RESOURCE_OUTPUT})
target_compile_options(${PROJECT_NAME} PRIVATE # shut the fuck up compiler
    $<$<CXX_COMPILER_ID:Clang>:-Wno-character-conversion>
)

add_dependencies(yellowcat98_modify serpentlua_res)
target_link_options(yellowcat98_modify PRIVATE ${RESOURCE_OUTPUT})

target_include_directories(yellowcat98_modify PRIVATE
    lua
    include
)

target_compile_definitions(${PROJECT_NAME} PUBLIC GEODE_MOD_ID="___yellowcat98modify") # required to compile some stuff in geode itself
target_link_libraries(${PROJECT_NAME} PUBLIC geode-sdk)