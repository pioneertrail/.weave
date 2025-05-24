#ifndef CHRONOVYAN_LEXER_H
#define CHRONOVYAN_LEXER_H

#include "token.h"
#include "source_file.h"
#include <memory>
#include <vector>

namespace chronovyan {

/**
 * @class Lexer
 * @brief Tokenizes Chronovyan source code into a stream of tokens
 */
class Lexer {
public:
    /**
     * @brief Construct a lexer for a given source file
     * @param sourceFile The source file to tokenize
     */
    explicit Lexer(std::shared_ptr<SourceFile> sourceFile);

    /**
     * @brief Get the next token from the source
     * @return The next token
     */
    Token nextToken();

    /**
     * @brief Peek at the next token without consuming it
     * @return The next token
     */
    const Token& peekToken();

    /**
     * @brief Peek at the token after the next one
     * @return The token after next
     */
    const Token& peekNextToken();

    /**
     * @brief Tokenize the entire source file
     * @return A vector of all tokens in the source
     */
    std::vector<Token> tokenizeAll();

    /**
     * @brief Get the source file being tokenized
     * @return The source file
     */
    std::shared_ptr<const SourceFile> getSourceFile() const;

private:
    std::shared_ptr<SourceFile> m_sourceFile;
    const std::string& m_source;
    size_t m_position = 0;
    size_t m_line = 1;
    size_t m_column = 1;
    Token m_currentToken;
    Token m_nextToken;
    bool m_hasNextToken = false;

    /**
     * @brief Scan a single token from the current position
     * @return The scanned token
     */
    Token scanToken();

    /**
     * @brief Create a token with the current location information
     * @param type The token type
     * @param lexeme The token text
     * @return A new token
     */
    Token makeToken(TokenType type, const std::string& lexeme);

    /**
     * @brief Create a token with the current location information
     * @param type The token type
     * @param start The start position of the lexeme
     * @param length The length of the lexeme
     * @return A new token
     */
    Token makeToken(TokenType type, size_t start, size_t length);

    /**
     * @brief Create an error token with a message
     * @param message The error message
     * @return An error token
     */
    Token errorToken(const std::string& message);

    /**
     * @brief Handle an identifier or keyword
     * @return The token for the identifier or keyword
     */
    Token scanIdentifier();

    /**
     * @brief Handle a number literal
     * @return The token for the number
     */
    Token scanNumber();

    /**
     * @brief Handle a string literal
     * @return The token for the string
     */
    Token scanString();

    /**
     * @brief Get the current character without advancing
     * @return The current character
     */
    char peek() const;

    /**
     * @brief Get the next character without advancing
     * @return The next character
     */
    char peekNext() const;

    /**
     * @brief Advance to the next character
     * @return The previous current character
     */
    char advance();

    /**
     * @brief Check if the current character matches expected and advance if it does
     * @param expected The character to match
     * @return True if the current character matched and was consumed
     */
    bool match(char expected);

    /**
     * @brief Skip whitespace and comments
     */
    void skipWhitespace();

    /**
     * @brief Check if we've reached the end of the source
     * @return True if at the end of the source
     */
    bool isAtEnd() const;

    /**
     * @brief Check if a character is a valid identifier start
     * @param c The character to check
     * @return True if the character can start an identifier
     */
    static bool isAlpha(char c);

    /**
     * @brief Check if a character is a digit
     * @param c The character to check
     * @return True if the character is a digit
     */
    static bool isDigit(char c);

    /**
     * @brief Check if a character is a valid identifier part
     * @param c The character to check
     * @return True if the character can be part of an identifier
     */
    static bool isAlphaNumeric(char c);

    /**
     * @brief Create a source location for the current position
     * @return A source location
     */
    SourceLocation makeLocation() const;

    /**
     * @brief Create a source location for a specific position
     * @param position The position in the source
     * @return A source location
     */
    SourceLocation makeLocation(size_t position) const;

    /**
     * @brief Create a source location for a range
     * @param start The start position
     * @param end The end position
     * @return A source location
     */
    SourceLocation makeLocation(size_t start, size_t end) const;
};

} // namespace chronovyan

#endif // CHRONOVYAN_LEXER_H 