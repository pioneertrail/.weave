#include "ast_nodes.h"
#include <stdexcept>

namespace chronovyan {

// LiteralExprNode

LiteralExprNode::LiteralExprNode(LiteralValue value)
    : m_value(std::move(value)) {}

void LiteralExprNode::accept(ASTVisitor& visitor) const {
    visitor.visitLiteralExpr(*this);
}

// VariableExprNode

VariableExprNode::VariableExprNode(std::string name)
    : m_name(std::move(name)) {}

void VariableExprNode::accept(ASTVisitor& visitor) const {
    visitor.visitVariableExpr(*this);
}

// UnaryExprNode

UnaryExprNode::UnaryExprNode(Token op, std::unique_ptr<ExprNode> right)
    : m_operator(std::move(op)), m_right(std::move(right)) {}

void UnaryExprNode::accept(ASTVisitor& visitor) const {
    visitor.visitUnaryExpr(*this);
}

// BinaryExprNode

BinaryExprNode::BinaryExprNode(
    std::unique_ptr<ExprNode> left,
    Token op,
    std::unique_ptr<ExprNode> right
) : m_left(std::move(left)), 
    m_operator(std::move(op)), 
    m_right(std::move(right)) {}

void BinaryExprNode::accept(ASTVisitor& visitor) const {
    visitor.visitBinaryExpr(*this);
}

// GroupingExprNode

GroupingExprNode::GroupingExprNode(std::unique_ptr<ExprNode> expression)
    : m_expression(std::move(expression)) {}

void GroupingExprNode::accept(ASTVisitor& visitor) const {
    visitor.visitGroupingExpr(*this);
}

// AssignExprNode

AssignExprNode::AssignExprNode(
    std::string name,
    std::unique_ptr<ExprNode> value
) : m_name(std::move(name)), 
    m_operator(Token(TokenType::EQUAL, "=", SourceLocation())), 
    m_value(std::move(value)) {}

AssignExprNode::AssignExprNode(
    std::string name,
    Token op,
    std::unique_ptr<ExprNode> value
) : m_name(std::move(name)), 
    m_operator(std::move(op)), 
    m_value(std::move(value)) {}

void AssignExprNode::accept(ASTVisitor& visitor) const {
    visitor.visitAssignExpr(*this);
}

// CallExprNode

CallExprNode::CallExprNode(
    std::unique_ptr<ExprNode> callee,
    std::vector<std::unique_ptr<ExprNode>> arguments
) : m_callee(std::move(callee)), m_arguments(std::move(arguments)) {}

void CallExprNode::accept(ASTVisitor& visitor) const {
    visitor.visitCallExpr(*this);
}

// ExprStmtNode

ExprStmtNode::ExprStmtNode(std::unique_ptr<ExprNode> expression)
    : m_expression(std::move(expression)) {}

void ExprStmtNode::accept(ASTVisitor& visitor) const {
    visitor.visitExprStmt(*this);
}

// BlockStmtNode

BlockStmtNode::BlockStmtNode(std::vector<std::unique_ptr<StmtNode>> statements)
    : m_statements(std::move(statements)) {}

void BlockStmtNode::accept(ASTVisitor& visitor) const {
    visitor.visitBlockStmt(*this);
}

// TypeNode

TypeNode::TypeNode(Token name)
    : m_name(std::move(name)) {}

void TypeNode::accept(ASTVisitor& visitor) const {
    visitor.visitType(*this);
}

// VariableDeclStmtNode

VariableDeclStmtNode::VariableDeclStmtNode(
    std::string name,
    std::unique_ptr<TypeNode> type,
    VariableModifier modifier,
    std::vector<VariableFlag> flags,
    std::unique_ptr<ExprNode> initializer
) : m_name(std::move(name)),
    m_type(std::move(type)),
    m_modifier(modifier),
    m_flags(std::move(flags)),
    m_initializer(std::move(initializer)) {}

void VariableDeclStmtNode::accept(ASTVisitor& visitor) const {
    visitor.visitVariableDeclStmt(*this);
}

const ExprNode& VariableDeclStmtNode::getInitializer() const {
    if (!m_initializer) {
        throw std::runtime_error("Variable declaration has no initializer");
    }
    return *m_initializer;
}

ExprNode& VariableDeclStmtNode::getInitializer() {
    if (!m_initializer) {
        throw std::runtime_error("Variable declaration has no initializer");
    }
    return *m_initializer;
}

// IfStmtNode

IfStmtNode::IfStmtNode(
    std::unique_ptr<ExprNode> condition,
    std::unique_ptr<StmtNode> thenBranch,
    std::unique_ptr<StmtNode> elseBranch
) : m_condition(std::move(condition)),
    m_thenBranch(std::move(thenBranch)),
    m_elseBranch(std::move(elseBranch)) {}

void IfStmtNode::accept(ASTVisitor& visitor) const {
    visitor.visitIfStmt(*this);
}

const StmtNode& IfStmtNode::getElseBranch() const {
    if (!m_elseBranch) {
        throw std::runtime_error("If statement has no else branch");
    }
    return *m_elseBranch;
}

StmtNode& IfStmtNode::getElseBranch() {
    if (!m_elseBranch) {
        throw std::runtime_error("If statement has no else branch");
    }
    return *m_elseBranch;
}

// TemporalOpStmtNode

TemporalOpStmtNode::TemporalOpStmtNode(
    TemporalOpType opType,
    std::vector<std::unique_ptr<ExprNode>> arguments,
    std::unique_ptr<BlockStmtNode> body
) : m_opType(opType),
    m_arguments(std::move(arguments)),
    m_body(std::move(body)) {}

void TemporalOpStmtNode::accept(ASTVisitor& visitor) const {
    visitor.visitTemporalOpStmt(*this);
}

// ProgramNode

ProgramNode::ProgramNode(std::vector<std::unique_ptr<StmtNode>> statements)
    : m_statements(std::move(statements)) {}

void ProgramNode::accept(ASTVisitor& visitor) const {
    visitor.visitProgram(*this);
}

} // namespace chronovyan 