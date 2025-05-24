#ifndef CHRONOVYAN_PARSER_H
#define CHRONOVYAN_PARSER_H

#include "lexer.h"
#include "ast_nodes.h"
#include "error_handler.h"
#include <memory>
#include <vector>
#include <functional>

namespace chronovyan {

/**
 * @class Parser
 * @brief Parses tokens into an Abstract Syntax Tree (AST)
 */
class Parser {
public:
    /**
     * @brief Construct a parser with a lexer
     * @param lexer The lexer to get tokens from
     */
    explicit Parser(std::shared_ptr<Lexer> lexer);

    /**
     * @brief Parse the source into a program AST
     * @return The root program node
     * @throws ChronovyanParseError if parsing fails
     */
    std::unique_ptr<ProgramNode> parse();

private:
    std::shared_ptr<Lexer> m_lexer;
    Token m_current;
    Token m_previous;
    bool m_hadError = false;
    bool m_panicMode = false;

    // Parsing methods for different grammar rules
    
    // Statements
    std::unique_ptr<StmtNode> declaration();
    std::unique_ptr<StmtNode> variableDeclaration();
    std::unique_ptr<StmtNode> statement();
    std::unique_ptr<StmtNode> expressionStatement();
    std::unique_ptr<BlockStmtNode> block();
    std::unique_ptr<IfStmtNode> ifStatement();
    std::unique_ptr<TemporalOpStmtNode> temporalOpStatement();

    // Types
    std::unique_ptr<TypeNode> typeDeclaration();

    // Expressions - using Pratt parsing for precedence
    std::unique_ptr<ExprNode> expression();
    std::unique_ptr<ExprNode> assignment();
    std::unique_ptr<ExprNode> logicalOr();
    std::unique_ptr<ExprNode> logicalAnd();
    std::unique_ptr<ExprNode> equality();
    std::unique_ptr<ExprNode> comparison();
    std::unique_ptr<ExprNode> term();
    std::unique_ptr<ExprNode> factor();
    std::unique_ptr<ExprNode> unary();
    std::unique_ptr<ExprNode> call();
    std::unique_ptr<ExprNode> primary();
    
    // Helper methods
    
    /**
     * @brief Finish a call expression by parsing arguments
     * @param callee The expression being called
     * @return A call expression node
     */
    std::unique_ptr<CallExprNode> finishCall(std::unique_ptr<ExprNode> callee);
    
    /**
     * @brief Parse a variable flag (::STATIC, ::VOLATILE, etc.)
     * @return The parsed flag
     */
    VariableFlag parseVariableFlag();
    
    /**
     * @brief Parse a variable modifier (CONF, REB)
     * @return The parsed modifier
     */
    VariableModifier parseVariableModifier();
    
    /**
     * @brief Consume the current token and advance if it matches the expected type
     * @param type The expected token type
     * @param errorMessage The error message if the token doesn't match
     * @return The consumed token
     * @throws ChronovyanParseError if the token doesn't match
     */
    Token consume(TokenType type, const std::string& errorMessage);
    
    /**
     * @brief Check if the current token is of the specified type
     * @param type The token type to check
     * @return True if the current token matches
     */
    bool check(TokenType type) const;
    
    /**
     * @brief Match the current token against any of the specified types and advance if matched
     * @param types The token types to match
     * @return True if the current token matched any of the types
     */
    template <typename... Args>
    bool match(Args... types) {
        return (matchOne(types) || ...);
    }
    
    /**
     * @brief Match the current token against a single type and advance if matched
     * @param type The token type to match
     * @return True if the current token matched
     */
    bool matchOne(TokenType type);
    
    /**
     * @brief Advance to the next token
     * @return The previous token
     */
    Token advance();
    
    /**
     * @brief Report a parse error at the current token
     * @param message The error message
     * @return A parse error (for convenience in expressions)
     */
    ChronovyanParseError error(const std::string& message);
    
    /**
     * @brief Report a parse error at a specific token
     * @param token The token where the error occurred
     * @param message The error message
     * @return A parse error (for convenience in expressions)
     */
    ChronovyanParseError error(const Token& token, const std::string& message);
    
    /**
     * @brief Recover from a parse error by synchronizing to a statement boundary
     */
    void synchronize();
};

} // namespace chronovyan

#endif // CHRONOVYAN_PARSER_H 