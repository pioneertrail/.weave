cmake_minimum_required(VERSION 3.14)
project(Chronovyan VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/source_file.cpp
    src/lexer.cpp
    src/token.cpp
    src/parser.cpp
    src/ast_nodes.cpp
    src/interpreter.cpp
    src/value.cpp
    src/environment.cpp
    src/temporal_runtime.cpp
    src/error_handler.cpp
    src/source_location.cpp
)

# Add executable
add_executable(chronovyan ${SOURCES})

# Add test executable if tests are enabled
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Install rules
install(TARGETS chronovyan DESTINATION bin) 