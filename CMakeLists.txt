cmake_minimum_required(VERSION 3.14)

# Use vcpkg if available
if(DEFINED ENV{VCPKG_ROOT})
    include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(chronovyan VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler options
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic)
    
    # Fix for MinGW std::variant issue
    if(MINGW)
        add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
    endif()
endif()

# Define include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Main executable
add_executable(chronovyan 
    src/main.cpp
    src/value.cpp
    src/lexer.cpp
    src/token.cpp
    src/parser.cpp
    src/interpreter.cpp
    src/error_handler.cpp
    src/environment.cpp
    src/source_file.cpp
    src/source_location.cpp
    src/ast_nodes.cpp
    src/temporal_runtime.cpp
)

# Option to build tests
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Install
install(TARGETS chronovyan
    RUNTIME DESTINATION bin
) 