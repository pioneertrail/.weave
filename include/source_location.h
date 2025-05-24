#ifndef CHRONOVYAN_SOURCE_LOCATION_H
#define CHRONOVYAN_SOURCE_LOCATION_H

#include <string>
#include <memory>

namespace chronovyan {

// Forward declaration to avoid circular includes
class SourceFile;

/**
 * @struct SourceLocation
 * @brief Represents a location in the source code (file, line, column)
 */
struct SourceLocation {
    std::shared_ptr<const SourceFile> sourceFile;
    size_t line;
    size_t column;
    size_t position; // Offset in source string

    /**
     * @brief Create an invalid/unknown source location
     */
    SourceLocation() : line(0), column(0), position(0) {}

    /**
     * @brief Create a source location with known coordinates
     */
    SourceLocation(std::shared_ptr<const SourceFile> file, size_t l, size_t c, size_t pos)
        : sourceFile(std::move(file)), line(l), column(c), position(pos) {}

    /**
     * @brief Check if this location is valid
     */
    bool isValid() const { return sourceFile != nullptr && line > 0 && column > 0; }

    /**
     * @brief Get a string representation of this location
     */
    std::string toString() const;
};

} // namespace chronovyan

#endif // CHRONOVYAN_SOURCE_LOCATION_H 