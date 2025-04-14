cmake_minimum_required(VERSION 3.10)
project(CodeEducator VERSION 0.1.0)

# C++ standard setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# # Architecture-specific flags for ARM - use more conservative settings
# if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
#     message(STATUS "ARM64 architecture detected, using conservative compile flags")
#     add_compile_options(-march=armv8-a -mtune=generic)
#     add_compile_options(-O0)  # Disable optimizations for debugging
# endif()

# Find pybind11
execute_process(
    COMMAND python3 -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
list(APPEND CMAKE_PREFIX_PATH ${PYBIND11_CMAKE_DIR})
find_package(pybind11 REQUIRED)

# Include directories for header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Flexibly handle different file structures - check which files exist
set(SOURCES "")

# Parser files - check both naming conventions
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/parser/CodeParser.cpp")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/parser/CodeParser.cpp")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/parser)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/parser/code_parser.cpp")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/parser/code_parser.cpp")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/parser)
else()
    message(FATAL_ERROR "Parser source file not found. Please check your directory structure.")
endif()

# Code analyzer files - check both naming conventions
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/parser/Analyzer.cpp")
	list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/parser/Analyzer.cpp")
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/analyzer)
endif()

# Bindings files - check both naming conventions
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/bindings/bindings.cpp")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/bindings/bindings.cpp")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/bindings/bindings.cpp")
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/bindings/bindings.cpp")
else()
    message(FATAL_ERROR "Bindings source file not found. Please check your directory structure.")
endif()

# Print found sources for debugging
message(STATUS "Using source files: ${SOURCES}")

# Python module
pybind11_add_module(code_educator_core ${SOURCES})

# Very conservative compile options for compatibility
target_compile_options(code_educator_core PRIVATE
    -fPIC
    -fno-strict-aliasing
)

# Installation
install(TARGETS code_educator_core DESTINATION .)
