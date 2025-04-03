cmake_minimum_required(VERSION 3.10)
project(CodeMentor VERSION 0.1.0)

# C++ standard setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# pybind11 setup
find_package(pybind11 REQUIRED)

# path headerfile
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set source files
set(SOURCES
    src/cpp/parser/code_parser.cpp
    src/cpp/bindings/bindings.cpp
)

# Python module
pybind11_add_module(codementor_core ${SOURCES})

# Compile options
if(MSVC)
    target_compile_options(codementor_core PRIVATE /W4)
else()
    target_compile_options(codementor_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# installation
install(TARGETS codementor_core DESTINATION .)
