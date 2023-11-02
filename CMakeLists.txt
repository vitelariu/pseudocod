cmake_minimum_required(VERSION 3.1...3.27)

project(Pseudocod LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(HEADER_FILES
    "${PROJECT_SOURCE_DIR}/src/interpreter.hpp"
    "${PROJECT_SOURCE_DIR}/src/lexer.hpp"
    "${PROJECT_SOURCE_DIR}/src/parser.hpp"
    "${PROJECT_SOURCE_DIR}/src/errors.hpp"
)
file(GLOB SOURCES "src/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(interpret ${SOURCES})

target_include_directories(interpret
    PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/src")
