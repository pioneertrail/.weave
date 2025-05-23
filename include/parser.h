// parser.h
#ifndef CHRONOVYAN_PARSER_H
#define CHRONOVYAN_PARSER_H

#include "lexer.h"
#include "ast_nodes.h"
#include <memory>
#include <vector>

namespace chronovyan {

class Parser {
public:
    Parser(std::shared_ptr<Lexer> lexer);
    ~Parser();

    std::unique_ptr<ProgramNode> parse();

private:
    std::shared_ptr<Lexer> m_lexer;
    Token m_current;
    Token m_previous;

    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    void consume(TokenType type, const std::string& message);
    void synchronize();
    
    // Parse methods for different grammar rules
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<StmtNode> parseStatement();
    std::unique_ptr<StmtNode> parseDeclaration();
    std::unique_ptr<VariableDeclStmtNode> parseVarDeclaration();
    std::unique_ptr<StmtNode> parseFunctionDeclaration();
    std::unique_ptr<BlockStmtNode> parseBlock();
    std::unique_ptr<IfStmtNode> parseIfStatement();
    std::unique_ptr<StmtNode> parseWhileStatement();
    std::unique_ptr<StmtNode> parseForStatement();
    std::unique_ptr<StmtNode> parseReturnStatement();
    std::unique_ptr<StmtNode> parsePrintStatement();
    std::unique_ptr<ExprStmtNode> parseExpressionStatement();
    std::unique_ptr<ExprNode> parseAssignment();
    std::unique_ptr<ExprNode> parseLogicalOr();
    std::unique_ptr<ExprNode> parseLogicalAnd();
    std::unique_ptr<ExprNode> parseEquality();
    std::unique_ptr<ExprNode> parseComparison();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
    std::unique_ptr<ExprNode> parseUnary();
    std::unique_ptr<ExprNode> parseCall();
    std::unique_ptr<ExprNode> parsePrimary();
};

} // namespace chronovyan

#endif // CHRONOVYAN_PARSER_H 