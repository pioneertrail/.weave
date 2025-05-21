# Find GTest package
find_package(GTest REQUIRED)

# Add test executable
add_executable(chronovyan_tests
    test_temporal_synchronizer.cpp
    test_ml_optimization.cpp
)

# Link against GTest and main library
target_link_libraries(chronovyan_tests
    PRIVATE
        chronovyan
        GTest::GTest
        GTest::Main
)

# Add tests
add_test(NAME chronovyan_tests COMMAND chronovyan_tests)

add_executable(ml_optimizer_smoke_test ml_optimizer_smoke_test.cpp)
target_link_libraries(ml_optimizer_smoke_test
    PRIVATE
    chronovyan
    GTest::gtest_main
) 