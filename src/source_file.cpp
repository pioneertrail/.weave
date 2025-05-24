#include "source_file.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace chronovyan {

SourceFile::SourceFile(const std::string& filename) : m_name(filename) {
    // Open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    // Read the entire file into m_source
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_source = buffer.str();
    
    // Index lines for efficient lookup
    indexLines();
}

SourceFile::SourceFile(std::string&& source, const std::string& sourceName) 
    : m_source(std::move(source)), m_name(sourceName) {
    // Index lines for efficient lookup
    indexLines();
}

const std::string& SourceFile::getSource() const {
    return m_source;
}

const std::string& SourceFile::getName() const {
    return m_name;
}

std::string SourceFile::getLine(size_t lineNumber) const {
    if (lineNumber == 0 || lineNumber > m_lineOffsets.size()) {
        throw std::out_of_range("Invalid line number: " + std::to_string(lineNumber));
    }
    
    size_t start = m_lineOffsets[lineNumber - 1];
    size_t end;
    
    if (lineNumber == m_lineOffsets.size()) {
        // Last line
        end = m_source.length();
    } else {
        // Not the last line
        end = m_lineOffsets[lineNumber];
        
        // Adjust end to exclude the newline character
        if (end > 0 && (m_source[end - 1] == '\n' || m_source[end - 1] == '\r')) {
            end--;
            
            // Handle CRLF
            if (end > 0 && m_source[end - 1] == '\r' && m_source[end] == '\n') {
                end--;
            }
        }
    }
    
    return m_source.substr(start, end - start);
}

size_t SourceFile::getPosition(size_t line, size_t column) const {
    if (line == 0 || line > m_lineOffsets.size()) {
        throw std::out_of_range("Invalid line number: " + std::to_string(line));
    }
    
    size_t lineStart = m_lineOffsets[line - 1];
    size_t lineEnd;
    
    if (line == m_lineOffsets.size()) {
        // Last line
        lineEnd = m_source.length();
    } else {
        // Not the last line
        lineEnd = m_lineOffsets[line];
    }
    
    if (column == 0 || column > (lineEnd - lineStart + 1)) {
        throw std::out_of_range("Invalid column number: " + std::to_string(column));
    }
    
    return lineStart + column - 1;
}

std::pair<size_t, size_t> SourceFile::getLineAndColumn(size_t position) const {
    if (position >= m_source.length()) {
        // If position is at the end of the file, return the last line and column
        if (position == m_source.length() && !m_source.empty()) {
            size_t line = m_lineOffsets.size();
            size_t column = position - m_lineOffsets[line - 1] + 1;
            return {line, column};
        }
        
        throw std::out_of_range("Invalid position: " + std::to_string(position));
    }
    
    // Find the line that contains the position
    auto it = std::lower_bound(m_lineOffsets.begin(), m_lineOffsets.end(), position);
    
    if (it == m_lineOffsets.begin()) {
        // Position is on the first line
        return {1, position + 1};
    }
    
    // Calculate line number and column
    size_t line = std::distance(m_lineOffsets.begin(), it);
    size_t lineStart = *(it - 1);
    size_t column = position - lineStart + 1;
    
    return {line, column};
}

void SourceFile::indexLines() {
    // Clear any existing line offsets
    m_lineOffsets.clear();
    
    // The first line always starts at position 0
    m_lineOffsets.push_back(0);
    
    // Find the start of each line
    for (size_t i = 0; i < m_source.length(); ++i) {
        if (m_source[i] == '\n') {
            // The next line starts after this newline
            m_lineOffsets.push_back(i + 1);
        }
    }
}

} // namespace chronovyan 