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

# Support for vcpkg integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

# If not set externally, try to find a local vcpkg installation
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
    message(STATUS "Using local vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
  endif()
endif()

# Setup tests only if requested
option(BUILD_TESTS "Build tests" ON)

# Find GTest package only if building tests
if(BUILD_TESTS)
    # Check if we're using MinGW
    if(MINGW)
        message(STATUS "Using MinGW - will use MinGW-compatible GTest through vcpkg")
        # For MinGW, prefer static linking to avoid DLL issues
        set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")
        
        # Set explicit GTest_DIR for MinGW builds if vcpkg is available
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-mingw-static/share/gtest")
            set(GTest_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-mingw-static/share/gtest"
                CACHE STRING "Path to GTest package configuration files")
            message(STATUS "Setting explicit GTest_DIR: ${GTest_DIR}")
        endif()
    endif()
    
    # Try to find GTest using CONFIG mode (preferred with vcpkg)
    find_package(GTest CONFIG)
    
    if(NOT GTest_FOUND)
        message(STATUS "GTest not found using CONFIG mode. This is expected if not using vcpkg.")
        message(STATUS "Falling back to MODULE mode, but this may not work with MinGW.")
        find_package(GTest MODULE)
        
        if(GTEST_FOUND)
            message(STATUS "GTest found using MODULE mode")
            # Include directories
            include_directories(${GTEST_INCLUDE_DIRS})
            # Set compatibility variables
            set(GTEST_LIBRARIES ${GTEST_BOTH_LIBRARIES})
        else()
            message(FATAL_ERROR "GTest not found. Please install GTest using vcpkg with 'vcpkg install gtest:x64-mingw-static'")
        endif()
    else()
        message(STATUS "GTest found using CONFIG mode (preferred)")
    endif()
    
    enable_testing()
    
    # Print diagnostic messages for GTest
    message(STATUS "GTest_FOUND: ${GTest_FOUND}")
    message(STATUS "GTEST_FOUND: ${GTEST_FOUND}")
    if(DEFINED GTEST_INCLUDE_DIRS)
        message(STATUS "GTEST_INCLUDE_DIRS: ${GTEST_INCLUDE_DIRS}")
    endif()
    if(DEFINED GTEST_LIBRARIES)
        message(STATUS "GTEST_LIBRARIES: ${GTEST_LIBRARIES}")
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
if(BUILD_TESTS AND (GTest_FOUND OR GTEST_FOUND))
    # Simple test executable

    # Link against GTest
    if(GTest_FOUND)
        # CONFIG mode - use imported targets

    else()
        # MODULE mode - use variables
        target_link_libraries(simple_gtest_test
            PRIVATE
                ${GTEST_LIBRARIES}
        )
    endif()

    # Add test
    add_test(NAME simple_gtest_test COMMAND simple_gtest_test)

    # Define a function to add our test targets with consistent flags
    function(add_chronovyan_test TEST_NAME TEST_SOURCE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})

        # Link against libraries
        target_link_libraries(${TEST_NAME} PRIVATE chronovyan)
        
        if(GTest_FOUND)
            # CONFIG mode - use imported targets
            target_link_libraries(${TEST_NAME}
                PRIVATE
                    GTest::gtest
                    GTest::gtest_main
                    GTest::gmock
                    GTest::gmock_main
            )
        else()
            # MODULE mode - use variables
            target_link_libraries(${TEST_NAME}
                PRIVATE
                    ${GTEST_LIBRARIES}
            )
        endif()

        # Add MinGW-specific link options if needed
        if(MINGW)
            target_link_libraries(${TEST_NAME} PRIVATE -static-libgcc -static-libstdc++)
            target_link_options(${TEST_NAME} PRIVATE "-Wl,--allow-multiple-definition")
        endif()

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
else()
    if(BUILD_TESTS AND NOT (GTest_FOUND OR GTEST_FOUND))
        message(WARNING "BUILD_TESTS is ON but GTest was not found. Tests will not be built.")
    endif()
endif() 