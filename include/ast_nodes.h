#ifndef CHRONOVYAN_AST_NODES_H
#define CHRONOVYAN_AST_NODES_H

#include "token.h"
#include "source_location.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

namespace chronovyan {

// Forward declarations
class ASTVisitor;

/**
 * @enum VariableModifier
 * @brief Defines the variable modifiers (CONF or REB)
 */
enum class VariableModifier {
    CONF, // Conformist path - deterministic, stable
    REB   // Rebel path - probabilistic, quantum
};

/**
 * @enum VariableFlag
 * @brief Defines the variable flags that modify behavior
 */
enum class VariableFlag {
    STATIC,   // Immutable once initialized
    VOLATILE, // Can change across timeline operations
    ANCHOR,   // Creates a fixed point in the timeline
    WEAVER,   // Can influence multiple timelines
    FLUX,     // Can have quantum fluctuations
    ECHO      // Retains history of previous values
};

/**
 * @enum TemporalOpType
 * @brief Defines the types of temporal operations
 */
enum class TemporalOpType {
    FOR_CHRONON,       // Temporal loop over chronons
    WHILE_EVENT,       // Loop until a timeline event occurs
    REWIND_FLOW,       // Rewind execution to a previous state
    BRANCH_TIMELINE,   // Create a new timeline branch
    MERGE_TIMELINES,   // Merge multiple timeline branches
    TEMPORAL_ECHO_LOOP // Loop with access to previous iterations
};

/**
 * @class ASTNode
 * @brief Base class for all AST nodes
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    /**
     * @brief Accept a visitor to process this node
     * @param visitor The visitor to accept
     */
    virtual void accept(ASTVisitor& visitor) const = 0;
    
    /**
     * @brief Get the source location of this node
     */
    const SourceLocation& getLocation() const { return m_location; }
    
    /**
     * @brief Set the source location of this node
     */
    void setLocation(SourceLocation location) { m_location = std::move(location); }

protected:
    SourceLocation m_location;
};

// Expression nodes

/**
 * @class ExprNode
 * @brief Base class for all expression nodes
 */
class ExprNode : public ASTNode {
public:
    virtual ~ExprNode() = default;
};

/**
 * @class LiteralExprNode
 * @brief Node for literal values (integers, floats, strings, booleans)
 */
class LiteralExprNode : public ExprNode {
public:
    /**
     * @brief Variant to hold different literal types
     */
    using LiteralValue = std::variant<int64_t, double, std::string, bool>;
    
    /**
     * @brief Construct a literal expression
     * @param value The literal value
     */
    explicit LiteralExprNode(LiteralValue value);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the literal value
     */
    const LiteralValue& getValue() const { return m_value; }

private:
    LiteralValue m_value;
};

/**
 * @class VariableExprNode
 * @brief Node for variable references
 */
class VariableExprNode : public ExprNode {
public:
    /**
     * @brief Construct a variable expression
     * @param name The variable name
     */
    explicit VariableExprNode(std::string name);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the variable name
     */
    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
};

/**
 * @class UnaryExprNode
 * @brief Node for unary expressions (e.g., -x, !y)
 */
class UnaryExprNode : public ExprNode {
public:
    /**
     * @brief Construct a unary expression
     * @param op The operator token
     * @param right The operand
     */
    UnaryExprNode(Token op, std::unique_ptr<ExprNode> right);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the operator token
     */
    const Token& getOperator() const { return m_operator; }
    
    /**
     * @brief Get the operand
     */
    const ExprNode& getRight() const { return *m_right; }
    
    /**
     * @brief Get the operand (mutable version)
     */
    ExprNode& getRight() { return *m_right; }

private:
    Token m_operator;
    std::unique_ptr<ExprNode> m_right;
};

/**
 * @class BinaryExprNode
 * @brief Node for binary expressions (e.g., a + b, x == y)
 */
class BinaryExprNode : public ExprNode {
public:
    /**
     * @brief Construct a binary expression
     * @param left The left operand
     * @param op The operator token
     * @param right The right operand
     */
    BinaryExprNode(
        std::unique_ptr<ExprNode> left,
        Token op,
        std::unique_ptr<ExprNode> right
    );
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the left operand
     */
    const ExprNode& getLeft() const { return *m_left; }
    
    /**
     * @brief Get the left operand (mutable version)
     */
    ExprNode& getLeft() { return *m_left; }
    
    /**
     * @brief Get the operator token
     */
    const Token& getOperator() const { return m_operator; }
    
    /**
     * @brief Get the right operand
     */
    const ExprNode& getRight() const { return *m_right; }
    
    /**
     * @brief Get the right operand (mutable version)
     */
    ExprNode& getRight() { return *m_right; }

private:
    std::unique_ptr<ExprNode> m_left;
    Token m_operator;
    std::unique_ptr<ExprNode> m_right;
};

/**
 * @class GroupingExprNode
 * @brief Node for grouped expressions with parentheses
 */
class GroupingExprNode : public ExprNode {
public:
    /**
     * @brief Construct a grouping expression
     * @param expression The contained expression
     */
    explicit GroupingExprNode(std::unique_ptr<ExprNode> expression);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the contained expression
     */
    const ExprNode& getExpression() const { return *m_expression; }
    
    /**
     * @brief Get the contained expression (mutable version)
     */
    ExprNode& getExpression() { return *m_expression; }

private:
    std::unique_ptr<ExprNode> m_expression;
};

/**
 * @class AssignExprNode
 * @brief Node for assignment expressions (e.g., a = b)
 */
class AssignExprNode : public ExprNode {
public:
    /**
     * @brief Construct an assignment expression
     * @param name The variable name
     * @param value The value to assign
     */
    AssignExprNode(std::string name, std::unique_ptr<ExprNode> value);
    
    /**
     * @brief Construct an assignment expression with an operator
     * @param name The variable name
     * @param op The assignment operator token (=, +=, -=, etc.)
     * @param value The value to assign
     */
    AssignExprNode(std::string name, Token op, std::unique_ptr<ExprNode> value);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the variable name
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * @brief Get the operator token
     */
    const Token& getOperator() const { return m_operator; }
    
    /**
     * @brief Get the value expression
     */
    const ExprNode& getValue() const { return *m_value; }
    
    /**
     * @brief Get the value expression (mutable version)
     */
    ExprNode& getValue() { return *m_value; }

private:
    std::string m_name;
    Token m_operator;
    std::unique_ptr<ExprNode> m_value;
};

/**
 * @class CallExprNode
 * @brief Node for function calls
 */
class CallExprNode : public ExprNode {
public:
    /**
     * @brief Construct a call expression
     * @param callee The expression to call
     * @param arguments The arguments to pass
     */
    CallExprNode(
        std::unique_ptr<ExprNode> callee,
        std::vector<std::unique_ptr<ExprNode>> arguments
    );
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the callee expression
     */
    const ExprNode& getCallee() const { return *m_callee; }
    
    /**
     * @brief Get the callee expression (mutable version)
     */
    ExprNode& getCallee() { return *m_callee; }
    
    /**
     * @brief Get the arguments
     */
    const std::vector<std::unique_ptr<ExprNode>>& getArguments() const { return m_arguments; }
    
    /**
     * @brief Get the arguments (mutable version)
     */
    std::vector<std::unique_ptr<ExprNode>>& getArguments() { return m_arguments; }

private:
    std::unique_ptr<ExprNode> m_callee;
    std::vector<std::unique_ptr<ExprNode>> m_arguments;
};

// Statement nodes

/**
 * @class StmtNode
 * @brief Base class for all statement nodes
 */
class StmtNode : public ASTNode {
public:
    virtual ~StmtNode() = default;
};

/**
 * @class ExprStmtNode
 * @brief Node for expression statements
 */
class ExprStmtNode : public StmtNode {
public:
    /**
     * @brief Construct an expression statement
     * @param expression The expression
     */
    explicit ExprStmtNode(std::unique_ptr<ExprNode> expression);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the expression
     */
    const ExprNode& getExpression() const { return *m_expression; }
    
    /**
     * @brief Get the expression (mutable version)
     */
    ExprNode& getExpression() { return *m_expression; }

private:
    std::unique_ptr<ExprNode> m_expression;
};

/**
 * @class BlockStmtNode
 * @brief Node for blocks of statements
 */
class BlockStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a block statement
     * @param statements The statements in the block
     */
    explicit BlockStmtNode(std::vector<std::unique_ptr<StmtNode>> statements);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the statements
     */
    const std::vector<std::unique_ptr<StmtNode>>& getStatements() const { return m_statements; }
    
    /**
     * @brief Get the statements (mutable version)
     */
    std::vector<std::unique_ptr<StmtNode>>& getStatements() { return m_statements; }

private:
    std::vector<std::unique_ptr<StmtNode>> m_statements;
};

/**
 * @class TypeNode
 * @brief Node for type declarations
 */
class TypeNode : public ASTNode {
public:
    /**
     * @brief Construct a type node
     * @param name The type name token
     */
    explicit TypeNode(Token name);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the type name token
     */
    const Token& getName() const { return m_name; }

private:
    Token m_name;
};

/**
 * @class VariableDeclStmtNode
 * @brief Node for variable declarations
 */
class VariableDeclStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a variable declaration
     * @param name The variable name
     * @param type The variable type
     * @param modifier The variable modifier (CONF/REB)
     * @param flags The variable flags
     * @param initializer The initial value (optional)
     */
    VariableDeclStmtNode(
        std::string name,
        std::unique_ptr<TypeNode> type,
        VariableModifier modifier,
        std::vector<VariableFlag> flags,
        std::unique_ptr<ExprNode> initializer
    );
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the variable name
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * @brief Get the variable type
     */
    const TypeNode& getType() const { return *m_type; }
    
    /**
     * @brief Get the variable type (mutable version)
     */
    TypeNode& getType() { return *m_type; }
    
    /**
     * @brief Get the variable modifier
     */
    VariableModifier getModifier() const { return m_modifier; }
    
    /**
     * @brief Get the variable flags
     */
    const std::vector<VariableFlag>& getFlags() const { return m_flags; }
    
    /**
     * @brief Check if the variable has an initializer
     */
    bool hasInitializer() const { return m_initializer != nullptr; }
    
    /**
     * @brief Get the initializer
     * @throws std::runtime_error if there is no initializer
     */
    const ExprNode& getInitializer() const;
    
    /**
     * @brief Get the initializer (mutable version)
     * @throws std::runtime_error if there is no initializer
     */
    ExprNode& getInitializer();

private:
    std::string m_name;
    std::unique_ptr<TypeNode> m_type;
    VariableModifier m_modifier;
    std::vector<VariableFlag> m_flags;
    std::unique_ptr<ExprNode> m_initializer;
};

/**
 * @class IfStmtNode
 * @brief Node for if statements
 */
class IfStmtNode : public StmtNode {
public:
    /**
     * @brief Construct an if statement
     * @param condition The condition expression
     * @param thenBranch The then branch
     * @param elseBranch The else branch (optional)
     */
    IfStmtNode(
        std::unique_ptr<ExprNode> condition,
        std::unique_ptr<StmtNode> thenBranch,
        std::unique_ptr<StmtNode> elseBranch = nullptr
    );
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the condition expression
     */
    const ExprNode& getCondition() const { return *m_condition; }
    
    /**
     * @brief Get the condition expression (mutable version)
     */
    ExprNode& getCondition() { return *m_condition; }
    
    /**
     * @brief Get the then branch
     */
    const StmtNode& getThenBranch() const { return *m_thenBranch; }
    
    /**
     * @brief Get the then branch (mutable version)
     */
    StmtNode& getThenBranch() { return *m_thenBranch; }
    
    /**
     * @brief Check if there is an else branch
     */
    bool hasElseBranch() const { return m_elseBranch != nullptr; }
    
    /**
     * @brief Get the else branch
     * @throws std::runtime_error if there is no else branch
     */
    const StmtNode& getElseBranch() const;
    
    /**
     * @brief Get the else branch (mutable version)
     * @throws std::runtime_error if there is no else branch
     */
    StmtNode& getElseBranch();

private:
    std::unique_ptr<ExprNode> m_condition;
    std::unique_ptr<StmtNode> m_thenBranch;
    std::unique_ptr<StmtNode> m_elseBranch;
};

/**
 * @class TemporalOpStmtNode
 * @brief Node for temporal operations
 */
class TemporalOpStmtNode : public StmtNode {
public:
    /**
     * @brief Construct a temporal operation statement
     * @param opType The type of temporal operation
     * @param arguments The arguments to the operation
     * @param body The body of the operation
     */
    TemporalOpStmtNode(
        TemporalOpType opType,
        std::vector<std::unique_ptr<ExprNode>> arguments,
        std::unique_ptr<BlockStmtNode> body
    );
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the operation type
     */
    TemporalOpType getOpType() const { return m_opType; }
    
    /**
     * @brief Get the arguments
     */
    const std::vector<std::unique_ptr<ExprNode>>& getArguments() const { return m_arguments; }
    
    /**
     * @brief Get the arguments (mutable version)
     */
    std::vector<std::unique_ptr<ExprNode>>& getArguments() { return m_arguments; }
    
    /**
     * @brief Get the body
     */
    const BlockStmtNode& getBody() const { return *m_body; }
    
    /**
     * @brief Get the body (mutable version)
     */
    BlockStmtNode& getBody() { return *m_body; }

private:
    TemporalOpType m_opType;
    std::vector<std::unique_ptr<ExprNode>> m_arguments;
    std::unique_ptr<BlockStmtNode> m_body;
};

/**
 * @class ProgramNode
 * @brief The root node of the AST
 */
class ProgramNode : public ASTNode {
public:
    /**
     * @brief Construct a program node
     * @param statements The top-level statements
     */
    explicit ProgramNode(std::vector<std::unique_ptr<StmtNode>> statements);
    
    void accept(ASTVisitor& visitor) const override;
    
    /**
     * @brief Get the statements
     */
    const std::vector<std::unique_ptr<StmtNode>>& getStatements() const { return m_statements; }
    
    /**
     * @brief Get the statements (mutable version)
     */
    std::vector<std::unique_ptr<StmtNode>>& getStatements() { return m_statements; }

private:
    std::vector<std::unique_ptr<StmtNode>> m_statements;
};

/**
 * @class ASTVisitor
 * @brief Interface for AST visitors
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expressions
    virtual void visitLiteralExpr(const LiteralExprNode& expr) = 0;
    virtual void visitVariableExpr(const VariableExprNode& expr) = 0;
    virtual void visitUnaryExpr(const UnaryExprNode& expr) = 0;
    virtual void visitBinaryExpr(const BinaryExprNode& expr) = 0;
    virtual void visitGroupingExpr(const GroupingExprNode& expr) = 0;
    virtual void visitAssignExpr(const AssignExprNode& expr) = 0;
    virtual void visitCallExpr(const CallExprNode& expr) = 0;
    
    // Statements
    virtual void visitExprStmt(const ExprStmtNode& stmt) = 0;
    virtual void visitBlockStmt(const BlockStmtNode& stmt) = 0;
    virtual void visitVariableDeclStmt(const VariableDeclStmtNode& stmt) = 0;
    virtual void visitIfStmt(const IfStmtNode& stmt) = 0;
    virtual void visitTemporalOpStmt(const TemporalOpStmtNode& stmt) = 0;
    
    // Other nodes
    virtual void visitType(const TypeNode& type) = 0;
    virtual void visitProgram(const ProgramNode& program) = 0;
};

} // namespace chronovyan

#endif // CHRONOVYAN_AST_NODES_H 