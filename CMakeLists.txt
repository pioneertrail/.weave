cmake_minimum_required(VERSION 3.14)
project(chronovyan VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set MSVC runtime library globally
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
endif()

# Determine the GTest installation path based on the main project's build type
if(CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
    set(GTEST_INSTALL_DIR "C:/gtest_standalone_build/install/debug")
elseif(CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]$" OR
       CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo]$" OR
       CMAKE_BUILD_TYPE MATCHES "^[Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll]$")
    set(GTEST_INSTALL_DIR "C:/gtest_standalone_build/install/release")
else()
    set(GTEST_INSTALL_DIR "C:/gtest_standalone_build/install/debug")
    message(STATUS "GTest: CMAKE_BUILD_TYPE is '${CMAKE_BUILD_TYPE}'. Defaulting to Debug GTest libraries.")
endif()

# Set GTest_DIR to the location of GTestConfig.cmake
set(GTest_DIR "${GTEST_INSTALL_DIR}/lib/cmake/GTest")
message(STATUS "Using GTest from: ${GTest_DIR}")

# Setup tests only if requested
option(BUILD_TESTS "Build tests" ON)

# Find GTest package only if building tests
if(BUILD_TESTS)
    # Try to find GTest package
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        message(STATUS "Could not find pre-built GTest, disabling tests")
        set(BUILD_TESTS OFF)
    else()
        # Enable testing
        enable_testing()
        
        # Set include directories
        include_directories(
            ${GTEST_INSTALL_DIR}/include  # Add this to ensure GTest headers are found
        )
        
        # Print diagnostic messages for GTest libraries
        message(STATUS "GTest include directory: ${GTEST_INSTALL_DIR}/include")
        message(STATUS "GTest library directory: ${GTEST_INSTALL_DIR}/lib")
        message(STATUS "GTest libraries found:")
        foreach(lib gtest gtest_main gmock gmock_main)
          if(EXISTS "${GTEST_INSTALL_DIR}/lib/${lib}.lib")
            message(STATUS "  ${lib}.lib - FOUND")
          else()
            message(STATUS "  ${lib}.lib - NOT FOUND")
          endif()
        endforeach()
    endif()
endif()

# Add the chronovyan library - make sure files are in the correct order
add_library(chronovyan
    src/notification_service.cpp  # This should go first as it has no dependencies
    src/metric_collector.cpp
    src/mode_decision_engine.cpp
    src/state_controller.cpp
    src/ml_model.cpp
    src/advanced_temporal_control.cpp
    src/core.cpp
    src/real_time_optimizer.cpp
    src/temporal_synchronizer.cpp
)

# Set include directories for the chronovyan library
target_include_directories(chronovyan PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add missing include directories for iostream, etc.
if(MSVC)
    target_compile_options(chronovyan PRIVATE "/W4")
else()
    target_compile_options(chronovyan PRIVATE "-Wall" "-Wextra")
endif()

# Only build tests if enabled
if(BUILD_TESTS)
    # Simple test executable
    add_executable(simple_gtest_test simple_gtest_test.cpp)
    
    # Set compiler flags for test to match GTest build
    if(MSVC)
        target_compile_options(simple_gtest_test PRIVATE "/MDd")
    elseif(MINGW)
        # Ensure stdlib is compatible with the prebuilt GTest
        target_link_libraries(simple_gtest_test PRIVATE -static-libgcc -static-libstdc++)
    endif()
    
    # Link against GTest libraries
    target_link_libraries(simple_gtest_test
        PRIVATE
            GTest::gtest_main
    )
    
    # Add test
    add_test(NAME simple_gtest_test COMMAND simple_gtest_test)
    
    # Define a function to add our test targets with consistent flags
    function(add_chronovyan_test TEST_NAME TEST_SOURCE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        
        # Set compiler flags for test to match GTest build
        if(MSVC)
            target_compile_options(${TEST_NAME} PRIVATE "/MDd")
        elseif(MINGW)
            # Ensure stdlib is compatible with the prebuilt GTest
            target_link_libraries(${TEST_NAME} PRIVATE -static-libgcc -static-libstdc++)
        endif()
        
        target_link_libraries(${TEST_NAME}
            PRIVATE
                chronovyan
                GTest::gtest
                GTest::gtest_main
                GTest::gmock
                GTest::gmock_main
        )
        
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endfunction()
    
    # System integration test
    add_chronovyan_test(system_integration_test tests/system_integration_test.cpp)
    
    # Metric collector test
    add_chronovyan_test(metric_collector_test tests/metric_collector_test.cpp)
    
    # Metric collector and mode decision integration test
    add_chronovyan_test(metric_collector_mode_decision_integration_test tests/metric_collector_mode_decision_integration_test.cpp)
    
    # Temporal synchronizer test
    add_chronovyan_test(temporal_synchronizer_test tests/temporal_synchronizer_test.cpp)
    
    # ML optimizer smoke test
    add_chronovyan_test(ml_optimizer_smoke_test tests/ml_optimizer_smoke_test.cpp)
    
    # Advanced temporal control test
    add_chronovyan_test(advanced_temporal_control_test tests/advanced_temporal_control_test.cpp)
endif() 