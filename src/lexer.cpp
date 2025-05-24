#include "lexer.h"
#include "error_handler.h"
#include <sstream>

namespace chronovyan {

Lexer::Lexer(std::shared_ptr<SourceFile> sourceFile)
    : m_sourceFile(std::move(sourceFile)), m_source(m_sourceFile->getSource()) {}

Token Lexer::nextToken() {
    if (m_hasNextToken) {
        m_currentToken = m_nextToken;
        m_hasNextToken = false;
        return m_currentToken;
    }
    
    skipWhitespace();
    
    if (isAtEnd()) {
        SourceLocation location = makeLocation();
        return m_currentToken = Token(TokenType::EOF_TOKEN, "", location);
    }
    
    m_currentToken = scanToken();
    return m_currentToken;
}

const Token& Lexer::peekToken() {
    if (!m_hasNextToken) {
        // Cache the next token
        size_t oldPosition = m_position;
        size_t oldLine = m_line;
        size_t oldColumn = m_column;
        
        m_nextToken = nextToken();
        
        // Restore the position
        m_position = oldPosition;
        m_line = oldLine;
        m_column = oldColumn;
        
        m_hasNextToken = true;
    }
    
    return m_nextToken;
}

const Token& Lexer::peekNextToken() {
    // Get the current state
    size_t oldPosition = m_position;
    size_t oldLine = m_line;
    size_t oldColumn = m_column;
    bool oldHasNextToken = m_hasNextToken;
    Token oldNextToken = m_nextToken;
    
    // Peek the next token
    peekToken();
    
    // Advance to consume the peeked token
    nextToken();
    
    // Peek the token after next
    const Token& tokenAfterNext = peekToken();
    
    // Restore the original state
    m_position = oldPosition;
    m_line = oldLine;
    m_column = oldColumn;
    m_hasNextToken = oldHasNextToken;
    m_nextToken = oldNextToken;
    
    return tokenAfterNext;
}

std::vector<Token> Lexer::tokenizeAll() {
    std::vector<Token> tokens;
    
    while (true) {
        Token token = nextToken();
        tokens.push_back(token);
        
        if (token.isType(TokenType::EOF_TOKEN)) {
            break;
        }
    }
    
    return tokens;
}

std::shared_ptr<const SourceFile> Lexer::getSourceFile() const {
    return m_sourceFile;
}

Token Lexer::scanToken() {
    char c = advance();
    
    if (isAlpha(c)) {
        return scanIdentifier();
    }
    
    if (isDigit(c)) {
        return scanNumber();
    }
    
    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': return makeToken(TokenType::RIGHT_PAREN, ")");
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case '[': return makeToken(TokenType::LEFT_BRACKET, "[");
        case ']': return makeToken(TokenType::RIGHT_BRACKET, "]");
        case ',': return makeToken(TokenType::COMMA, ",");
        case '.': return makeToken(TokenType::DOT, ".");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        
        case ':':
            if (match(':')) {
                return makeToken(TokenType::DOUBLE_COLON, "::");
            }
            return makeToken(TokenType::COLON, ":");
            
        case '+':
            if (match('=')) {
                return makeToken(TokenType::PLUS_EQUAL, "+=");
            }
            return makeToken(TokenType::PLUS, "+");
            
        case '-':
            if (match('>')) {
                return makeToken(TokenType::TIMELINE_BRANCH, "->");
            } else if (match('=')) {
                return makeToken(TokenType::MINUS_EQUAL, "-=");
            }
            return makeToken(TokenType::MINUS, "-");
            
        case '*':
            if (match('=')) {
                return makeToken(TokenType::STAR_EQUAL, "*=");
            }
            return makeToken(TokenType::STAR, "*");
            
        case '/':
            if (match('/')) {
                // Comment, read until end of line
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                return scanToken();  // Skip the comment and get the next token
            } else if (match('*')) {
                // Multi-line comment, read until */
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') {
                        m_line++;
                        m_column = 1;
                    }
                    advance();
                }
                
                // Consume the */
                if (!isAtEnd()) {
                    advance();  // *
                    advance();  // /
                }
                
                return scanToken();  // Skip the comment and get the next token
            } else if (match('=')) {
                return makeToken(TokenType::SLASH_EQUAL, "/=");
            }
            return makeToken(TokenType::SLASH, "/");
            
        case '%':
            if (match('=')) {
                return makeToken(TokenType::PERCENT_EQUAL, "%=");
            }
            return makeToken(TokenType::PERCENT, "%");
            
        case '!':
            if (match('=')) {
                return makeToken(TokenType::BANG_EQUAL, "!=");
            }
            return makeToken(TokenType::BANG, "!");
            
        case '=':
            if (match('=')) {
                return makeToken(TokenType::EQUAL_EQUAL, "==");
            }
            return makeToken(TokenType::EQUAL, "=");
            
        case '<':
            if (match('=')) {
                return makeToken(TokenType::LESS_EQUAL, "<=");
            } else if (match('-')) {
                return makeToken(TokenType::TIMELINE_MERGE, "<-");
            }
            return makeToken(TokenType::LESS, "<");
            
        case '>':
            if (match('=')) {
                return makeToken(TokenType::GREATER_EQUAL, ">=");
            }
            return makeToken(TokenType::GREATER, ">");
            
        case '&':
            if (match('&')) {
                return makeToken(TokenType::AND, "&&");
            }
            return errorToken("Unexpected character '&'");
            
        case '|':
            if (match('|')) {
                return makeToken(TokenType::OR, "||");
            }
            return errorToken("Unexpected character '|'");
            
        case '?':
            if (match(':')) {
                return makeToken(TokenType::TEMPORAL_QUERY, "?:");
            }
            return errorToken("Unexpected character '?'");
            
        case '"': return scanString();
        
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            // Whitespace, should have been skipped
            return scanToken();
            
        default:
            std::stringstream ss;
            ss << "Unexpected character '" << c << "'";
            return errorToken(ss.str());
    }
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    SourceLocation location = makeLocation(m_position - lexeme.length(), m_position);
    return Token(type, lexeme, location);
}

Token Lexer::makeToken(TokenType type, size_t start, size_t length) {
    std::string lexeme = m_source.substr(start, length);
    SourceLocation location = makeLocation(start, start + length);
    return Token(type, lexeme, location);
}

Token Lexer::errorToken(const std::string& message) {
    SourceLocation location = makeLocation();
    ErrorHandler::getInstance().reportError(location, message);
    return Token(TokenType::ERROR, message, location);
}

Token Lexer::scanIdentifier() {
    size_t start = m_position - 1;  // -1 because we already consumed the first character
    
    while (isAlphaNumeric(peek())) {
        advance();
    }
    
    size_t length = m_position - start;
    std::string text = m_source.substr(start, length);
    
    // Check if it's a keyword
    auto& keywords = getKeywordMap();
    auto it = keywords.find(text);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    
    return makeToken(type, start, length);
}

Token Lexer::scanNumber() {
    size_t start = m_position - 1;  // -1 because we already consumed the first digit
    TokenType type = TokenType::INTEGER_LITERAL;
    
    while (isDigit(peek())) {
        advance();
    }
    
    // Look for a decimal point
    if (peek() == '.' && isDigit(peekNext())) {
        type = TokenType::FLOAT_LITERAL;
        
        // Consume the dot
        advance();
        
        // Consume the decimal part
        while (isDigit(peek())) {
            advance();
        }
    }
    
    // Look for an exponent
    if (peek() == 'e' || peek() == 'E') {
        type = TokenType::FLOAT_LITERAL;
        
        // Consume the 'e'
        advance();
        
        // Check for a sign
        if (peek() == '+' || peek() == '-') {
            advance();
        }
        
        // There must be at least one digit
        if (!isDigit(peek())) {
            return errorToken("Invalid number: expected digits after exponent");
        }
        
        // Consume the exponent digits
        while (isDigit(peek())) {
            advance();
        }
    }
    
    size_t length = m_position - start;
    return makeToken(type, start, length);
}

Token Lexer::scanString() {
    size_t start = m_position;  // Start after the opening quote
    
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            m_line++;
            m_column = 1;
        }
        
        // Handle escape sequences
        if (peek() == '\\' && !isAtEnd()) {
            advance();  // Consume the backslash
        }
        
        advance();
    }
    
    if (isAtEnd()) {
        return errorToken("Unterminated string");
    }
    
    // Consume the closing quote
    advance();
    
    size_t length = m_position - start - 1;  // -1 to exclude the closing quote
    return makeToken(TokenType::STRING_LITERAL, start, length);
}

char Lexer::peek() const {
    if (isAtEnd()) {
        return '\0';
    }
    return m_source[m_position];
}

char Lexer::peekNext() const {
    if (m_position + 1 >= m_source.length()) {
        return '\0';
    }
    return m_source[m_position + 1];
}

char Lexer::advance() {
    char c = m_source[m_position++];
    
    if (c == '\n') {
        m_line++;
        m_column = 1;
    } else {
        m_column++;
    }
    
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || m_source[m_position] != expected) {
        return false;
    }
    
    m_position++;
    m_column++;
    return true;
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
                
            case '\n':
                advance();  // advance will increment line and reset column
                break;
                
            case '/':
                if (peekNext() == '/') {
                    // Comment, read until end of line
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else if (peekNext() == '*') {
                    // Multi-line comment, read until */
                    advance();  // /
                    advance();  // *
                    
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            m_line++;
                            m_column = 1;
                        }
                        advance();
                    }
                    
                    // Consume the */
                    if (!isAtEnd()) {
                        advance();  // *
                        advance();  // /
                    }
                } else {
                    return;  // Not a comment, so we're done
                }
                break;
                
            default:
                return;  // Not whitespace, so we're done
        }
    }
}

bool Lexer::isAtEnd() const {
    return m_position >= m_source.length();
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

SourceLocation Lexer::makeLocation() const {
    return makeLocation(m_position - 1, m_position);
}

SourceLocation Lexer::makeLocation(size_t position) const {
    auto [line, column] = m_sourceFile->getLineAndColumn(position);
    return SourceLocation(m_sourceFile, line, column, position);
}

SourceLocation Lexer::makeLocation(size_t start, size_t end) const {
    // Note: 'end' parameter is intentionally unused in this implementation
    // but kept for future extensions or compatibility
    auto [line, column] = m_sourceFile->getLineAndColumn(start);
    return SourceLocation(m_sourceFile, line, column, start);
}

} // namespace chronovyan 