#include "interpreter.h"
#include "error_handler.h"
#include <stdexcept>
#include <sstream>

namespace chronovyan {

Interpreter::Interpreter() 
    : m_globals(std::make_shared<Environment>()),
      m_environment(m_globals),
      m_runtime(std::make_shared<TemporalRuntime>())
{
    defineNativeFunctions();
}

Value Interpreter::interpret(const ProgramNode& program) {
    try {
        visitProgram(program);
        return m_lastValue;
    } catch (const ChronovyanException& e) {
        // Already handled by the error system
        return Value(); // Return nil
    } catch (const std::exception& e) {
        // Unexpected error
        ErrorHandler::getInstance().reportError(SourceLocation(), 
            "Runtime error: " + std::string(e.what()));
        return Value(); // Return nil
    }
}

void Interpreter::execute(const StmtNode& stmt) {
    stmt.accept(*this);
}

Value Interpreter::evaluate(const ExprNode& expr) {
    expr.accept(*this);
    return m_lastValue;
}

std::shared_ptr<TemporalRuntime> Interpreter::getRuntime() const {
    return m_runtime;
}

std::shared_ptr<Environment> Interpreter::getGlobalEnvironment() const {
    return m_globals;
}

std::shared_ptr<Environment> Interpreter::getCurrentEnvironment() const {
    return m_environment;
}

// Visitor methods for expressions

void Interpreter::visitLiteralExpr(const LiteralExprNode& expr) {
    auto value = expr.getValue();
    
    if (std::holds_alternative<int64_t>(value)) {
        m_lastValue = Value(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        m_lastValue = Value(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        m_lastValue = Value(std::get<std::string>(value));
    } else if (std::holds_alternative<bool>(value)) {
        m_lastValue = Value(std::get<bool>(value));
    } else {
        // Should never happen
        m_lastValue = Value(); // nil
    }
}

void Interpreter::visitVariableExpr(const VariableExprNode& expr) {
    m_lastValue = lookUpVariable(expr.getName(), expr.getLocation());
}

void Interpreter::visitUnaryExpr(const UnaryExprNode& expr) {
    Value right = evaluate(expr.getRight());
    
    switch (expr.getOperator().type) {
        case TokenType::MINUS:
            m_lastValue = negate(right);
            break;
            
        case TokenType::BANG:
            m_lastValue = logicalNot(right);
            break;
            
        default:
            // Should never happen if the parser is correct
            throw std::runtime_error("Unknown unary operator");
    }
}

void Interpreter::visitBinaryExpr(const BinaryExprNode& expr) {
    Value left = evaluate(expr.getLeft());
    Value right = evaluate(expr.getRight());
    
    switch (expr.getOperator().type) {
        case TokenType::PLUS:
            m_lastValue = add(left, right);
            break;
            
        case TokenType::MINUS:
            m_lastValue = subtract(left, right);
            break;
            
        case TokenType::STAR:
            m_lastValue = multiply(left, right);
            break;
            
        case TokenType::SLASH:
            m_lastValue = divide(left, right);
            break;
            
        case TokenType::PERCENT:
            m_lastValue = modulo(left, right);
            break;
            
        case TokenType::EQUAL_EQUAL:
            m_lastValue = Value(areEqual(left, right));
            break;
            
        case TokenType::BANG_EQUAL:
            m_lastValue = Value(!areEqual(left, right));
            break;
            
        case TokenType::LESS:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() < right.asFloat());
            } else {
                throw std::runtime_error("Less than operator requires numeric operands");
            }
            break;
            
        case TokenType::LESS_EQUAL:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() <= right.asFloat());
            } else {
                throw std::runtime_error("Less than or equal operator requires numeric operands");
            }
            break;
            
        case TokenType::GREATER:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() > right.asFloat());
            } else {
                throw std::runtime_error("Greater than operator requires numeric operands");
            }
            break;
            
        case TokenType::GREATER_EQUAL:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() >= right.asFloat());
            } else {
                throw std::runtime_error("Greater than or equal operator requires numeric operands");
            }
            break;
            
        default:
            // Should never happen if the parser is correct
            throw std::runtime_error("Unknown binary operator");
    }
    
    // Update paradox level based on CONF/REB interaction
    updateParadoxLevel(left, right, expr.getOperator().type);
}

void Interpreter::visitGroupingExpr(const GroupingExprNode& expr) {
    m_lastValue = evaluate(expr.getExpression());
}

void Interpreter::visitAssignExpr(const AssignExprNode& expr) {
    Value value = evaluate(expr.getValue());
    
    // Handle variable assignment
    m_environment->assign(expr.getName(), value);
    m_lastValue = value;
}

void Interpreter::visitCallExpr(const CallExprNode& expr) {
    // Placeholder for function calls
    // This would evaluate the callee and arguments, then execute the function
    m_lastValue = Value(); // nil for now
}

// Visitor methods for statements

void Interpreter::visitExprStmt(const ExprStmtNode& stmt) {
    m_lastValue = evaluate(stmt.getExpression());
}

void Interpreter::visitBlockStmt(const BlockStmtNode& stmt) {
    executeBlock(stmt, std::make_shared<Environment>(m_environment));
}

void Interpreter::visitVariableDeclStmt(const VariableDeclStmtNode& stmt) {
    Value value;
    
    if (stmt.hasInitializer()) {
        value = evaluate(stmt.getInitializer());
    } else {
        // Default value based on type
        // For now, just use nil
        value = Value();
    }
    
    // Set variable modifiers and flags
    value.setModifier(stmt.getModifier());
    for (const auto& flag : stmt.getFlags()) {
        value.addFlag(flag);
    }
    
    // Define the variable in the current environment
    m_environment->define(stmt.getName(), value);
}

void Interpreter::visitIfStmt(const IfStmtNode& stmt) {
    Value condition = evaluate(stmt.getCondition());
    
    if (condition.asBoolean()) {
        execute(stmt.getThenBranch());
    } else if (stmt.hasElseBranch()) {
        execute(stmt.getElseBranch());
    }
}

void Interpreter::visitTemporalOpStmt(const TemporalOpStmtNode& stmt) {
    switch (stmt.getOpType()) {
        case TemporalOpType::FOR_CHRONON:
            executeForChronon(stmt);
            break;
            
        case TemporalOpType::WHILE_EVENT:
            executeWhileEvent(stmt);
            break;
            
        case TemporalOpType::REWIND_FLOW:
            executeRewindFlow(stmt);
            break;
            
        case TemporalOpType::BRANCH_TIMELINE:
            executeBranchTimeline(stmt);
            break;
            
        case TemporalOpType::MERGE_TIMELINES:
            executeMergeTimelines(stmt);
            break;
            
        case TemporalOpType::TEMPORAL_ECHO_LOOP:
            executeTemporalEchoLoop(stmt);
            break;
    }
}

// Visitor methods for other nodes

void Interpreter::visitType(const TypeNode& type) {
    // Nothing to do here - types don't produce values at runtime
}

void Interpreter::visitProgram(const ProgramNode& program) {
    for (const auto& stmt : program.getStatements()) {
        execute(*stmt);
    }
}

// Helper methods

void Interpreter::executeBlock(const BlockStmtNode& block, std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = m_environment;
    
    try {
        m_environment = environment;
        
        for (const auto& stmt : block.getStatements()) {
            execute(*stmt);
        }
    } catch (...) {
        m_environment = previous;
        throw;
    }
    
    m_environment = previous;
}

Value Interpreter::lookUpVariable(const std::string& name, const SourceLocation& location) {
    try {
        return m_environment->get(name);
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(location, 
            "Undefined variable '" + name + "'");
        throw;
    }
}

// Placeholder implementations for temporal operations

void Interpreter::executeForChronon(const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeWhileEvent(const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeRewindFlow(const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeBranchTimeline(const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeMergeTimelines(const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeTemporalEchoLoop(const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::defineNativeFunctions() {
    // Define native functions here
}

void Interpreter::updateParadoxLevel(const Value& left, const Value& right, TokenType operation) {
    // Placeholder for paradox management
}

Value Interpreter::handleVariableInteraction(const Value& left, const Value& right, TokenType operation) {
    // Placeholder for CONF/REB interaction
    return Value();
}

} // namespace chronovyan 