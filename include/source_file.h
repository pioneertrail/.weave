#ifndef CHRONOVYAN_SOURCE_FILE_H
#define CHRONOVYAN_SOURCE_FILE_H

#include <string>
#include <vector>

namespace chronovyan {

/**
 * @class SourceFile
 * @brief Represents a source code file with utility methods for line/column tracking
 */
class SourceFile {
public:
    /**
     * @brief Construct a SourceFile from a file on disk
     * @param filename The path to the file
     * @throws std::runtime_error if the file cannot be opened
     */
    explicit SourceFile(const std::string& filename);

    /**
     * @brief Construct a SourceFile from a string (e.g., for REPL input)
     * @param source The source code as a string
     * @param sourceName A name for the source (for error reporting)
     */
    explicit SourceFile(std::string&& source, const std::string& sourceName = "<string>");

    /**
     * @brief Get the entire source code
     * @return The source code as a string
     */
    const std::string& getSource() const;

    /**
     * @brief Get the name of the source
     * @return The source name
     */
    const std::string& getName() const;

    /**
     * @brief Get a specific line from the source
     * @param lineNumber The 1-based line number
     * @return The line as a string
     * @throws std::out_of_range if the line number is invalid
     */
    std::string getLine(size_t lineNumber) const;

    /**
     * @brief Get the position (offset) for a line and column
     * @param line The 1-based line number
     * @param column The 1-based column number
     * @return The 0-based offset in the source string
     */
    size_t getPosition(size_t line, size_t column) const;

    /**
     * @brief Get the line and column for a position
     * @param position The 0-based offset in the source string
     * @return A pair of (line, column) both 1-based
     */
    std::pair<size_t, size_t> getLineAndColumn(size_t position) const;

private:
    std::string m_source;
    std::string m_name;
    std::vector<size_t> m_lineOffsets;

    /**
     * @brief Index all line start positions for fast lookup
     */
    void indexLines();
};

} // namespace chronovyan

#endif // CHRONOVYAN_SOURCE_FILE_H 