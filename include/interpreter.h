#ifndef CHRONOVYAN_INTERPRETER_H
#define CHRONOVYAN_INTERPRETER_H

#include "ast_nodes.h"
#include "environment.h"
#include "temporal_runtime.h"
#include <memory>
#include <stack>

namespace chronovyan {

/**
 * @class Interpreter
 * @brief Interprets and executes Chronovyan AST nodes
 */
class Interpreter : public ASTVisitor {
public:
    /**
     * @brief Create a new interpreter
     */
    Interpreter();
    
    /**
     * @brief Interpret a program
     * @param program The program to interpret
     * @return The result of the last expression, or nil
     */
    Value interpret(const ProgramNode& program);
    
    /**
     * @brief Execute a single statement
     * @param stmt The statement to execute
     */
    void execute(const StmtNode& stmt);
    
    /**
     * @brief Evaluate an expression
     * @param expr The expression to evaluate
     * @return The result of the expression
     */
    Value evaluate(const ExprNode& expr);
    
    /**
     * @brief Get the temporal runtime
     */
    std::shared_ptr<TemporalRuntime> getRuntime() const;
    
    /**
     * @brief Get the global environment
     */
    std::shared_ptr<Environment> getGlobalEnvironment() const;
    
    /**
     * @brief Get the current environment
     */
    std::shared_ptr<Environment> getCurrentEnvironment() const;
    
private:
    std::shared_ptr<Environment> m_globals;
    std::shared_ptr<Environment> m_environment;
    std::shared_ptr<TemporalRuntime> m_runtime;
    Value m_lastValue;  // Result of the last expression evaluated
    
    // For handling return statements, breaks, and continues
    std::stack<Value> m_returnValues;
    bool m_isReturning = false;
    bool m_isBreaking = false;
    bool m_isContinuing = false;
    
    // Visitor methods for expressions
    void visitLiteralExpr(const LiteralExprNode& expr) override;
    void visitVariableExpr(const VariableExprNode& expr) override;
    void visitUnaryExpr(const UnaryExprNode& expr) override;
    void visitBinaryExpr(const BinaryExprNode& expr) override;
    void visitGroupingExpr(const GroupingExprNode& expr) override;
    void visitAssignExpr(const AssignExprNode& expr) override;
    void visitCallExpr(const CallExprNode& expr) override;
    
    // Visitor methods for statements
    void visitExprStmt(const ExprStmtNode& stmt) override;
    void visitBlockStmt(const BlockStmtNode& stmt) override;
    void visitVariableDeclStmt(const VariableDeclStmtNode& stmt) override;
    void visitIfStmt(const IfStmtNode& stmt) override;
    void visitTemporalOpStmt(const TemporalOpStmtNode& stmt) override;
    
    // Visitor methods for other nodes
    void visitType(const TypeNode& type) override;
    void visitProgram(const ProgramNode& program) override;
    
    // Helper methods for executing blocks and evaluating variables
    void executeBlock(const BlockStmtNode& block, std::shared_ptr<Environment> environment);
    Value lookUpVariable(const std::string& name, const SourceLocation& location);
    
    // Helper methods for handling CONF/REB interactions
    Value handleVariableInteraction(const Value& left, const Value& right, TokenType operation);
    void updateParadoxLevel(const Value& left, const Value& right, TokenType operation);
    
    // Helper methods for handling temporal operations
    void executeForChronon(const TemporalOpStmtNode& stmt);
    void executeWhileEvent(const TemporalOpStmtNode& stmt);
    void executeRewindFlow(const TemporalOpStmtNode& stmt);
    void executeBranchTimeline(const TemporalOpStmtNode& stmt);
    void executeMergeTimelines(const TemporalOpStmtNode& stmt);
    void executeTemporalEchoLoop(const TemporalOpStmtNode& stmt);
    
    // Native function definitions
    void defineNativeFunctions();
};

} // namespace chronovyan

#endif // CHRONOVYAN_INTERPRETER_H 