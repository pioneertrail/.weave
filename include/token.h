#ifndef CHRONOVYAN_TOKEN_H
#define CHRONOVYAN_TOKEN_H

#include "source_location.h"
#include <string>
#include <unordered_map>

namespace chronovyan {

/**
 * @enum TokenType
 * @brief Defines all possible token types in Chronovyan
 */
enum class TokenType {
    // End of file
    EOF_TOKEN,
    
    // Error token
    ERROR,

    // Identifiers and literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,

    // Keywords
    DECLARE,
    IF,
    ELSE,
    FOR_CHRONON,
    WHILE_EVENT,
    REWIND_FLOW,
    BRANCH_TIMELINE,
    MERGE_TIMELINES,
    TEMPORAL_ECHO_LOOP,
    PARADOX_HANDLER,
    TRUE,
    FALSE,
    COLLAPSE,
    
    // Variable modifiers
    CONF,
    REB,
    
    // Variable flags
    STATIC,
    VOLATILE,
    ANCHOR,
    WEAVER,
    FLUX,
    ECHO,
    
    // Types
    INT,
    FLOAT,
    BOOLEAN,
    STRING,
    VOID,
    ARRAY,
    MAP,
    TUPLE,
    TIMESTAMP,
    QUANTUM_STATE,

    // Operators
    PLUS,          // +
    MINUS,         // -
    STAR,          // *
    SLASH,         // /
    PERCENT,       // %
    
    // Comparison operators
    EQUAL_EQUAL,   // ==
    BANG_EQUAL,    // !=
    LESS,          // <
    LESS_EQUAL,    // <=
    GREATER,       // >
    GREATER_EQUAL, // >=
    
    // Assignment operators
    EQUAL,         // =
    PLUS_EQUAL,    // +=
    MINUS_EQUAL,   // -=
    STAR_EQUAL,    // *=
    SLASH_EQUAL,   // /=
    PERCENT_EQUAL, // %=
    
    // Logical operators
    AND,           // &&
    OR,            // ||
    BANG,          // !
    
    // Delimiters
    LEFT_PAREN,    // (
    RIGHT_PAREN,   // )
    LEFT_BRACE,    // {
    RIGHT_BRACE,   // }
    LEFT_BRACKET,  // [
    RIGHT_BRACKET, // ]
    COMMA,         // ,
    DOT,           // .
    COLON,         // :
    SEMICOLON,     // ;
    DOUBLE_COLON,  // ::
    
    // Temporal operators
    TIMELINE_BRANCH, // ->
    TIMELINE_MERGE,  // <-
    TEMPORAL_QUERY,  // ?:
};

/**
 * @struct Token
 * @brief Represents a token in the Chronovyan language
 */
struct Token {
    TokenType type;          // The type of token
    std::string lexeme;      // The raw text of the token
    SourceLocation location; // Where the token appears
    
    /**
     * @brief Construct a token with known properties
     */
    Token(
        TokenType tokenType,
        std::string tokenLexeme,
        SourceLocation tokenLocation
    ) : type(tokenType), 
        lexeme(std::move(tokenLexeme)), 
        location(std::move(tokenLocation)) {}
    
    /**
     * @brief Default constructor creates an ERROR token
     */
    Token() : type(TokenType::ERROR), lexeme(""), location() {}

    /**
     * @brief Check if this token is of a specific type
     */
    bool isType(TokenType t) const { return type == t; }

    /**
     * @brief Get a string representation of the token type
     */
    std::string typeToString() const;

    /**
     * @brief Get a string representation of this token
     */
    std::string toString() const;
};

/**
 * @brief Get a mapping of keywords to token types
 */
const std::unordered_map<std::string, TokenType>& getKeywordMap();

} // namespace chronovyan

#endif // CHRONOVYAN_TOKEN_H 