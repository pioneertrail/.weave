#include "source_file.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace chronovyan {

SourceFile::SourceFile(const std::string& filename)
    : m_name(filename)
{
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_source = buffer.str();
    
    indexLines();
}

SourceFile::SourceFile(std::string&& source, const std::string& sourceName)
    : m_source(std::move(source)), m_name(sourceName)
{
    indexLines();
}

const std::string& SourceFile::getSource() const {
    return m_source;
}

const std::string& SourceFile::getName() const {
    return m_name;
}

std::string SourceFile::getLine(size_t lineNumber) const {
    if (lineNumber < 1 || lineNumber > m_lineOffsets.size()) {
        throw std::out_of_range("Line number out of range: " + std::to_string(lineNumber));
    }
    
    size_t start = m_lineOffsets[lineNumber - 1];
    size_t end;
    
    if (lineNumber == m_lineOffsets.size()) {
        // Last line
        end = m_source.length();
    } else {
        // Not the last line
        end = m_lineOffsets[lineNumber];
        
        // Don't include the newline character
        if (end > 0 && (m_source[end - 1] == '\n' || m_source[end - 1] == '\r')) {
            --end;
            // Handle CRLF
            if (end > 0 && m_source[end - 1] == '\r' && m_source[end] == '\n') {
                --end;
            }
        }
    }
    
    return m_source.substr(start, end - start);
}

size_t SourceFile::getPosition(size_t line, size_t column) const {
    if (line < 1 || line > m_lineOffsets.size()) {
        throw std::out_of_range("Line number out of range: " + std::to_string(line));
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
    
    if (column < 1 || lineStart + column - 1 > lineEnd) {
        throw std::out_of_range("Column number out of range: " + std::to_string(column));
    }
    
    return lineStart + column - 1;
}

std::pair<size_t, size_t> SourceFile::getLineAndColumn(size_t position) const {
    if (position >= m_source.length()) {
        // If position is at the end of the file, return the last line and its length
        size_t lastLine = m_lineOffsets.size();
        size_t lastLineStart = m_lineOffsets[lastLine - 1];
        return {lastLine, position - lastLineStart + 1};
    }
    
    // Binary search to find the line containing position
    auto it = std::upper_bound(m_lineOffsets.begin(), m_lineOffsets.end(), position);
    
    if (it == m_lineOffsets.begin()) {
        // Position is before the first line
        return {1, position + 1};
    }
    
    size_t line = std::distance(m_lineOffsets.begin(), it);
    size_t lineStart = *(it - 1);
    
    return {line, position - lineStart + 1};
}

void SourceFile::indexLines() {
    m_lineOffsets.clear();
    m_lineOffsets.push_back(0); // First line starts at index 0
    
    for (size_t i = 0; i < m_source.length(); ++i) {
        if (m_source[i] == '\n') {
            m_lineOffsets.push_back(i + 1);
        } else if (m_source[i] == '\r') {
            if (i + 1 < m_source.length() && m_source[i + 1] == '\n') {
                // CRLF - skip the CR
                i++;
            }
            m_lineOffsets.push_back(i + 1);
        }
    }
    
    if (m_lineOffsets.size() == 1 && !m_source.empty()) {
        // If there are no newlines, add the end of the string
        m_lineOffsets.push_back(m_source.length());
    }
}

} // namespace chronovyan 