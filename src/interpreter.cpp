#include "interpreter.h"
#include "error_handler.h"
#include <sstream>
#include <ctime>
#include <iostream>

namespace chronovyan {

Interpreter::Interpreter() {
    // Initialize the global environment
    m_globals = std::make_shared<Environment>();
    
    // Set the current environment to the global environment
    m_environment = m_globals;
    
    // Initialize the temporal runtime
    m_runtime = std::make_shared<TemporalRuntime>(m_globals);
    
    // Define native functions
    defineNativeFunctions();
}

Value Interpreter::interpret(const ProgramNode& program) {
    try {
        // Reset state
        m_lastValue = Value();
        m_isReturning = false;
        m_isBreaking = false;
        m_isContinuing = false;
        
        // Visit the program node
        program.accept(*this);
        
        return m_lastValue;
    } catch (const ChronovyanRuntimeError& e) {
        // Report runtime errors
        ErrorHandler::getInstance().reportError(
            e.getLocation(),
            e.what(),
            ErrorSeverity::ERROR
        );
        
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
    // Create a Value object from the literal value
    if (const auto* intValue = std::get_if<int64_t>(&expr.getValue())) {
        m_lastValue = Value(*intValue);
    } else if (const auto* doubleValue = std::get_if<double>(&expr.getValue())) {
        m_lastValue = Value(*doubleValue);
    } else if (const auto* stringValue = std::get_if<std::string>(&expr.getValue())) {
        m_lastValue = Value(*stringValue);
    } else if (const auto* boolValue = std::get_if<bool>(&expr.getValue())) {
        m_lastValue = Value(*boolValue);
    } else {
        // This should never happen
        m_lastValue = Value();
    }
}

void Interpreter::visitVariableExpr(const VariableExprNode& expr) {
    m_lastValue = lookUpVariable(expr.getName(), expr.getLocation());
}

void Interpreter::visitUnaryExpr(const UnaryExprNode& expr) {
    // Evaluate the operand
    Value right = evaluate(expr.getRight());
    
    // Apply the operator
    switch (expr.getOperator().type) {
        case TokenType::MINUS:
            m_lastValue = negate(right);
            break;
            
        case TokenType::BANG:
            m_lastValue = logicalNot(right);
            break;
            
        default:
            // This should never happen if the parser is correct
            throw ChronovyanRuntimeError(
                "Invalid unary operator",
                expr.getLocation()
            );
    }
}

void Interpreter::visitBinaryExpr(const BinaryExprNode& expr) {
    // Evaluate the operands
    Value left = evaluate(expr.getLeft());
    Value right = evaluate(expr.getRight());
    
    // Apply the operator
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
            // Simple implementation for now
            if (!left.isNumeric() || !right.isNumeric()) {
                throw ChronovyanRuntimeError(
                    "Operands must be numbers",
                    expr.getLocation()
                );
            }
            
            if (left.isInteger() && right.isInteger()) {
                m_lastValue = Value(left.asInteger() < right.asInteger());
            } else {
                m_lastValue = Value(left.asFloat() < right.asFloat());
            }
            break;
            
        case TokenType::LESS_EQUAL:
            if (!left.isNumeric() || !right.isNumeric()) {
                throw ChronovyanRuntimeError(
                    "Operands must be numbers",
                    expr.getLocation()
                );
            }
            
            if (left.isInteger() && right.isInteger()) {
                m_lastValue = Value(left.asInteger() <= right.asInteger());
            } else {
                m_lastValue = Value(left.asFloat() <= right.asFloat());
            }
            break;
            
        case TokenType::GREATER:
            if (!left.isNumeric() || !right.isNumeric()) {
                throw ChronovyanRuntimeError(
                    "Operands must be numbers",
                    expr.getLocation()
                );
            }
            
            if (left.isInteger() && right.isInteger()) {
                m_lastValue = Value(left.asInteger() > right.asInteger());
            } else {
                m_lastValue = Value(left.asFloat() > right.asFloat());
            }
            break;
            
        case TokenType::GREATER_EQUAL:
            if (!left.isNumeric() || !right.isNumeric()) {
                throw ChronovyanRuntimeError(
                    "Operands must be numbers",
                    expr.getLocation()
                );
            }
            
            if (left.isInteger() && right.isInteger()) {
                m_lastValue = Value(left.asInteger() >= right.asInteger());
            } else {
                m_lastValue = Value(left.asFloat() >= right.asFloat());
            }
            break;
            
        case TokenType::AND:
            // Short-circuit evaluation
            if (!left.asBoolean()) {
                m_lastValue = left; // Already false
            } else {
                m_lastValue = Value(right.asBoolean());
            }
            break;
            
        case TokenType::OR:
            // Short-circuit evaluation
            if (left.asBoolean()) {
                m_lastValue = left; // Already true
            } else {
                m_lastValue = Value(right.asBoolean());
            }
            break;
            
        default:
            // This should never happen if the parser is correct
            throw ChronovyanRuntimeError(
                "Invalid binary operator",
                expr.getLocation()
            );
    }
    
    // Update paradox level for CONF/REB interactions
    updateParadoxLevel(left, right, expr.getOperator().type);
}

void Interpreter::visitGroupingExpr(const GroupingExprNode& expr) {
    m_lastValue = evaluate(expr.getExpression());
}

void Interpreter::visitAssignExpr(const AssignExprNode& expr) {
    // Evaluate the value to assign
    Value value = evaluate(expr.getValue());
    
    // Assign the value to the variable
    m_environment->assign(expr.getName(), value);
    
    // The result of an assignment expression is the assigned value
    m_lastValue = value;
}

void Interpreter::visitCallExpr(const CallExprNode& expr) {
    // Evaluate the callee
    Value callee = evaluate(expr.getCallee());
    
    // Evaluate the arguments
    std::vector<Value> arguments;
    for (const auto& arg : expr.getArguments()) {
        arguments.push_back(evaluate(*arg));
    }
    
    // Check if the callee is callable
    if (callee.isNativeFunction()) {
        // Call the native function
        m_lastValue = callee.asNativeFunction()(arguments);
    } else if (callee.isChronovyanFunction()) {
        // Call the Chronovyan function (not implemented yet)
        throw ChronovyanRuntimeError(
            "Chronovyan functions are not implemented yet",
            expr.getLocation()
        );
    } else {
        throw ChronovyanRuntimeError(
            "Can only call functions",
            expr.getLocation()
        );
    }
}

// Visitor methods for statements

void Interpreter::visitExprStmt(const ExprStmtNode& stmt) {
    m_lastValue = evaluate(stmt.getExpression());
}

void Interpreter::visitBlockStmt(const BlockStmtNode& stmt) {
    // Create a new environment for the block
    auto blockEnv = std::make_shared<Environment>(m_environment);
    
    // Execute the block with the new environment
    executeBlock(stmt, blockEnv);
}

void Interpreter::visitVariableDeclStmt(const VariableDeclStmtNode& stmt) {
    // Evaluate the initializer if present
    Value initialValue;
    if (stmt.hasInitializer()) {
        initialValue = evaluate(stmt.getInitializer());
    }
    
    // Set the variable modifier
    initialValue.setModifier(stmt.getModifier());
    
    // Set the variable flags
    for (const auto& flag : stmt.getFlags()) {
        initialValue.addFlag(flag);
    }
    
    // Define the variable in the current environment
    m_environment->define(stmt.getName(), initialValue);
}

void Interpreter::visitIfStmt(const IfStmtNode& stmt) {
    // Evaluate the condition
    Value condition = evaluate(stmt.getCondition());
    
    // Execute the appropriate branch
    if (condition.asBoolean()) {
        execute(stmt.getThenBranch());
    } else if (stmt.hasElseBranch()) {
        execute(stmt.getElseBranch());
    }
}

void Interpreter::visitTemporalOpStmt(const TemporalOpStmtNode& stmt) {
    // Execute the appropriate temporal operation
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
    // Usually nothing to do for type nodes during interpretation
}

void Interpreter::visitProgram(const ProgramNode& program) {
    // Execute each statement in the program
    for (const auto& stmt : program.getStatements()) {
        execute(*stmt);
    }
}

// Helper methods

void Interpreter::executeBlock(const BlockStmtNode& block, std::shared_ptr<Environment> environment) {
    // Save the current environment
    auto previousEnv = m_environment;
    
    try {
        // Set the current environment to the block's environment
        m_environment = environment;
        
        // Execute each statement in the block
        for (const auto& stmt : block.getStatements()) {
            execute(*stmt);
            
            // Check for control flow statements
            if (m_isReturning || m_isBreaking || m_isContinuing) {
                break;
            }
        }
    } catch (...) {
        // Restore the previous environment
        m_environment = previousEnv;
        throw;
    }
    
    // Restore the previous environment
    m_environment = previousEnv;
}

Value Interpreter::lookUpVariable(const std::string& name, const SourceLocation& location) {
    // Try to find the variable in the current environment chain
    try {
        return m_environment->get(name);
    } catch (const ChronovyanRuntimeError&) {
        throw ChronovyanRuntimeError(
            "Undefined variable '" + name + "'",
            location
        );
    }
}

void Interpreter::updateParadoxLevel(const Value& left, const Value& right, TokenType operation) {
    // Increase paradox level for CONF/REB interactions
    if (left.getModifier() != right.getModifier()) {
        // The amount to increase depends on the operation
        double amount = 0.1; // Default
        
        switch (operation) {
            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::STAR:
            case TokenType::SLASH:
            case TokenType::PERCENT:
                amount = 0.2; // Arithmetic operations increase paradox more
                break;
                
            default:
                amount = 0.1;
                break;
        }
        
        m_runtime->increaseParadoxLevel(amount);
    }
}

// Temporal operation implementations

void Interpreter::executeForChronon(const TemporalOpStmtNode& stmt) {
    // Not implemented yet
    throw ChronovyanRuntimeError(
        "FOR_CHRONON not implemented yet",
        stmt.getLocation()
    );
}

void Interpreter::executeWhileEvent(const TemporalOpStmtNode& stmt) {
    // Not implemented yet
    throw ChronovyanRuntimeError(
        "WHILE_EVENT not implemented yet",
        stmt.getLocation()
    );
}

void Interpreter::executeRewindFlow(const TemporalOpStmtNode& stmt) {
    // Not implemented yet
    throw ChronovyanRuntimeError(
        "REWIND_FLOW not implemented yet",
        stmt.getLocation()
    );
}

void Interpreter::executeBranchTimeline(const TemporalOpStmtNode& stmt) {
    // Not implemented yet
    throw ChronovyanRuntimeError(
        "BRANCH_TIMELINE not implemented yet",
        stmt.getLocation()
    );
}

void Interpreter::executeMergeTimelines(const TemporalOpStmtNode& stmt) {
    // Not implemented yet
    throw ChronovyanRuntimeError(
        "MERGE_TIMELINES not implemented yet",
        stmt.getLocation()
    );
}

void Interpreter::executeTemporalEchoLoop(const TemporalOpStmtNode& stmt) {
    // Not implemented yet
    throw ChronovyanRuntimeError(
        "TEMPORAL_ECHO_LOOP not implemented yet",
        stmt.getLocation()
    );
}

void Interpreter::defineNativeFunctions() {
    // Define some basic native functions
    
    // print function
    NativeFunction printFn = [](const std::vector<Value>& args) -> Value {
        for (const auto& arg : args) {
            std::cout << arg.toString();
        }
        std::cout << std::endl;
        return Value();
    };
    
    // Define the print function in the global environment
    m_globals->define("print", Value(printFn));
    
    // clock function (returns current time in seconds since epoch)
    NativeFunction clockFn = [](const std::vector<Value>& args) -> Value {
        return Value(static_cast<double>(std::time(nullptr)));
    };
    
    // Define the clock function in the global environment
    m_globals->define("clock", Value(clockFn));
}

} // namespace chronovyan 