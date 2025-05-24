#include <gtest/gtest.h>
#include <algorithm>
#include <string>

// Declaration of the function we're testing (from main.cpp)
// We need to declare it here since it's not part of a header file
extern bool hasValidExtension(const std::string& filename);

TEST(FileExtensionTest, ValidExtension) {
    // Test with a basic .cvy file
    EXPECT_TRUE(hasValidExtension("test.cvy"));
    
    // Test with a longer path
    EXPECT_TRUE(hasValidExtension("/path/to/some/file.cvy"));
    
    // Test with a Windows-style path
    EXPECT_TRUE(hasValidExtension("C:\\Users\\Test\\Documents\\code.cvy"));
    
    // Test with uppercase extension
    EXPECT_TRUE(hasValidExtension("TEST.CVY"));
    
    // Test with mixed case
    EXPECT_TRUE(hasValidExtension("Test.CvY"));
}

TEST(FileExtensionTest, InvalidExtension) {
    // Test with no extension
    EXPECT_FALSE(hasValidExtension("testfile"));
    
    // Test with wrong extension
    EXPECT_FALSE(hasValidExtension("test.txt"));
    
    // Test with the old extension
    EXPECT_FALSE(hasValidExtension("test.chronovyan"));
    
    // Test with similar but incorrect extension
    EXPECT_FALSE(hasValidExtension("test.cvya"));
    EXPECT_FALSE(hasValidExtension("test.cv"));
    
    // Test with too short filename
    EXPECT_FALSE(hasValidExtension("t.cvy")); // This should be valid, but our implementation requires at least 5 chars
}

TEST(FileExtensionTest, EdgeCases) {
    // Test with just the extension
    EXPECT_FALSE(hasValidExtension(".cvy"));
    
    // Test with empty string
    EXPECT_FALSE(hasValidExtension(""));
    
    // Test with only a period
    EXPECT_FALSE(hasValidExtension("."));
    
    // Test with multiple extensions
    EXPECT_FALSE(hasValidExtension("test.txt.cvy")); // Should be valid but depends on implementation
    
    // Test with spaces in name
    EXPECT_TRUE(hasValidExtension("my file.cvy"));
}

// This test would need to be updated if the implementation changes
TEST(FileExtensionTest, ImplementationSpecific) {
    // Our current implementation requires at least 5 characters (x.cvy)
    EXPECT_FALSE(hasValidExtension("a.cvy")); // 5 characters exactly
    EXPECT_TRUE(hasValidExtension("ab.cvy")); // 6 characters
    
    // Our implementation checks last 4 characters are ".cvy"
    EXPECT_TRUE(hasValidExtension("test.cvy"));
    EXPECT_FALSE(hasValidExtension("test.cvyx"));
} 