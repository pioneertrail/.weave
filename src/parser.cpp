#include "parser.h"
#include "error_handler.h"
#include <sstream>
#include <unordered_set>

namespace chronovyan {

Parser::Parser(std::shared_ptr<Lexer> lexer) : m_lexer(std::move(lexer)) {
    // Initialize by loading the first token
    advance();
}

std::unique_ptr<ProgramNode> Parser::parse() {
    try {
        std::vector<std::unique_ptr<StmtNode>> statements;
        
        // Parse declarations until we hit the end of the file
        while (!check(TokenType::EOF_TOKEN)) {
            statements.push_back(declaration());
        }
        
        // Create the program node with all top-level statements
        auto program = std::make_unique<ProgramNode>(std::move(statements));
        
        // If we had any errors during parsing, throw
        if (m_hadError) {
            throw ChronovyanParseError("Failed to parse program due to previous errors", SourceLocation());
        }
        
        return program;
    } catch (const ChronovyanParseError& e) {
        // If we're already in panic mode, we've handled this error, so just propagate it
        if (m_panicMode) throw;
        
        // Otherwise, enter panic mode and synchronize to next statement boundary
        m_panicMode = true;
        m_hadError = true;
        synchronize();
        
        throw; // Re-throw after synchronizing
    }
}

// Statement Parsing Methods

std::unique_ptr<StmtNode> Parser::declaration() {
    try {
        // Check for variable declaration
        if (match(TokenType::DECLARE)) {
            return variableDeclaration();
        }
        
        // If it's not a special declaration, it's a regular statement
        return statement();
    } catch (const ChronovyanParseError& e) {
        // Handle errors in declarations by synchronizing and returning nullptr
        synchronize();
        return nullptr; // This null statement will be filtered out later
    }
}

std::unique_ptr<StmtNode> Parser::variableDeclaration() {
    // Parse variable modifiers (CONF or REB)
    VariableModifier modifier = parseVariableModifier();
    
    // Parse variable name
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name after 'DECLARE'");
    std::string name = nameToken.lexeme;
    
    // Parse type
    consume(TokenType::COLON, "Expected ':' after variable name");
    auto type = typeDeclaration();
    
    // Parse flags if present
    std::vector<VariableFlag> flags;
    while (match(TokenType::DOUBLE_COLON)) {
        flags.push_back(parseVariableFlag());
    }
    
    // Parse initializer if present
    std::unique_ptr<ExprNode> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }
    
    // Require semicolon
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    // Create variable declaration node
    auto varDecl = std::make_unique<VariableDeclStmtNode>(
        name, std::move(type), modifier, flags, std::move(initializer));
    
    // Set source location
    varDecl->setLocation(nameToken.location);
    
    return varDecl;
}

std::unique_ptr<StmtNode> Parser::statement() {
    // Parse if statement
    if (match(TokenType::IF)) {
        return ifStatement();
    }
    
    // Parse temporal operation statements
    if (match(TokenType::FOR_CHRONON, TokenType::WHILE_EVENT, 
              TokenType::REWIND_FLOW, TokenType::BRANCH_TIMELINE,
              TokenType::MERGE_TIMELINES, TokenType::TEMPORAL_ECHO_LOOP)) {
        return temporalOpStatement();
    }
    
    // Parse block statement
    if (match(TokenType::LEFT_BRACE)) {
        return block();
    }
    
    // Default to expression statement
    return expressionStatement();
}

std::unique_ptr<StmtNode> Parser::expressionStatement() {
    // Parse the expression
    auto expr = expression();
    
    // Require semicolon
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    
    // Create expression statement node
    auto stmt = std::make_unique<ExprStmtNode>(std::move(expr));
    
    // Set source location
    stmt->setLocation(m_previous.location);
    
    return stmt;
}

std::unique_ptr<BlockStmtNode> Parser::block() {
    std::vector<std::unique_ptr<StmtNode>> statements;
    
    // Keep track of the opening brace location
    auto location = m_previous.location;
    
    // Parse statements until we hit the closing brace
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
        auto stmt = declaration();
        if (stmt) { // Only add non-null statements
            statements.push_back(std::move(stmt));
        }
    }
    
    // Require closing brace
    consume(TokenType::RIGHT_BRACE, "Expected '}' after block");
    
    // Create block statement node
    auto blockStmt = std::make_unique<BlockStmtNode>(std::move(statements));
    
    // Set source location
    blockStmt->setLocation(location);
    
    return blockStmt;
}

std::unique_ptr<IfStmtNode> Parser::ifStatement() {
    // Keep track of the if keyword location
    auto location = m_previous.location;
    
    // Parse condition
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'IF'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition");
    
    // Parse then branch
    auto thenBranch = statement();
    
    // Parse else branch if present
    std::unique_ptr<StmtNode> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }
    
    // Create if statement node
    auto ifStmt = std::make_unique<IfStmtNode>(
        std::move(condition), std::move(thenBranch), std::move(elseBranch));
    
    // Set source location
    ifStmt->setLocation(location);
    
    return ifStmt;
}

std::unique_ptr<TemporalOpStmtNode> Parser::temporalOpStatement() {
    // Determine temporal operation type based on the token
    TemporalOpType opType;
    switch (m_previous.type) {
        case TokenType::FOR_CHRONON:
            opType = TemporalOpType::FOR_CHRONON;
            break;
        case TokenType::WHILE_EVENT:
            opType = TemporalOpType::WHILE_EVENT;
            break;
        case TokenType::REWIND_FLOW:
            opType = TemporalOpType::REWIND_FLOW;
            break;
        case TokenType::BRANCH_TIMELINE:
            opType = TemporalOpType::BRANCH_TIMELINE;
            break;
        case TokenType::MERGE_TIMELINES:
            opType = TemporalOpType::MERGE_TIMELINES;
            break;
        case TokenType::TEMPORAL_ECHO_LOOP:
            opType = TemporalOpType::TEMPORAL_ECHO_LOOP;
            break;
        default:
            throw error("Invalid temporal operation type");
    }
    
    // Keep track of the operation keyword location
    auto location = m_previous.location;
    
    // Parse arguments
    consume(TokenType::LEFT_PAREN, "Expected '(' after temporal operation keyword");
    
    std::vector<std::unique_ptr<ExprNode>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after temporal operation arguments");
    
    // Parse body
    auto body = std::make_unique<BlockStmtNode>(std::vector<std::unique_ptr<StmtNode>>());
    if (match(TokenType::LEFT_BRACE)) {
        body = block();
    } else {
        throw error("Expected '{' to begin temporal operation body");
    }
    
    // Create temporal operation statement node
    auto temporalOp = std::make_unique<TemporalOpStmtNode>(
        opType, std::move(arguments), std::move(body));
    
    // Set source location
    temporalOp->setLocation(location);
    
    return temporalOp;
}

// Type Parsing Methods

std::unique_ptr<TypeNode> Parser::typeDeclaration() {
    // Parse type name
    Token typeToken = consume(
        TokenType::IDENTIFIER, 
        "Expected type name after ':'"
    );
    
    // TODO: Handle more complex types like arrays, maps, etc.
    
    // Create type node
    auto type = std::make_unique<TypeNode>(typeToken);
    
    // Set source location
    type->setLocation(typeToken.location);
    
    return type;
}

// Expression Parsing Methods - Using Pratt Parsing for precedence

std::unique_ptr<ExprNode> Parser::expression() {
    return assignment();
}

std::unique_ptr<ExprNode> Parser::assignment() {
    // Parse the left-hand side of the assignment
    auto expr = logicalOr();
    
    // Check if this is an assignment
    if (match(TokenType::EQUAL, TokenType::PLUS_EQUAL, 
              TokenType::MINUS_EQUAL, TokenType::STAR_EQUAL,
              TokenType::SLASH_EQUAL, TokenType::PERCENT_EQUAL)) {
        Token op = m_previous;
        auto value = assignment(); // Assignments are right-associative
        
        // Verify the left-hand side is a valid assignment target
        if (dynamic_cast<VariableExprNode*>(expr.get())) {
            auto variable = dynamic_cast<VariableExprNode*>(expr.get());
            auto result = std::make_unique<AssignExprNode>(
                variable->getName(), op, std::move(value));
            result->setLocation(expr->getLocation());
            return result;
        }
        
        // Invalid assignment target
        error(op, "Invalid assignment target");
    }
    
    return expr;
}

std::unique_ptr<ExprNode> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match(TokenType::OR)) {
        Token op = m_previous;
        auto right = logicalAnd();
        auto location = expr->getLocation();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
        expr->setLocation(location);
    }
    
    return expr;
}

std::unique_ptr<ExprNode> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match(TokenType::AND)) {
        Token op = m_previous;
        auto right = equality();
        auto location = expr->getLocation();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
        expr->setLocation(location);
    }
    
    return expr;
}

std::unique_ptr<ExprNode> Parser::equality() {
    auto expr = comparison();
    
    while (match(TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL)) {
        Token op = m_previous;
        auto right = comparison();
        auto location = expr->getLocation();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
        expr->setLocation(location);
    }
    
    return expr;
}

std::unique_ptr<ExprNode> Parser::comparison() {
    auto expr = term();
    
    while (match(TokenType::LESS, TokenType::LESS_EQUAL,
                 TokenType::GREATER, TokenType::GREATER_EQUAL)) {
        Token op = m_previous;
        auto right = term();
        auto location = expr->getLocation();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
        expr->setLocation(location);
    }
    
    return expr;
}

std::unique_ptr<ExprNode> Parser::term() {
    auto expr = factor();
    
    while (match(TokenType::PLUS, TokenType::MINUS)) {
        Token op = m_previous;
        auto right = factor();
        auto location = expr->getLocation();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
        expr->setLocation(location);
    }
    
    return expr;
}

std::unique_ptr<ExprNode> Parser::factor() {
    auto expr = unary();
    
    while (match(TokenType::STAR, TokenType::SLASH, TokenType::PERCENT)) {
        Token op = m_previous;
        auto right = unary();
        auto location = expr->getLocation();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
        expr->setLocation(location);
    }
    
    return expr;
}

std::unique_ptr<ExprNode> Parser::unary() {
    if (match(TokenType::BANG, TokenType::MINUS)) {
        Token op = m_previous;
        auto right = unary();
        auto location = op.location;
        auto expr = std::make_unique<UnaryExprNode>(op, std::move(right));
        expr->setLocation(location);
        return expr;
    }
    
    return call();
}

std::unique_ptr<ExprNode> Parser::call() {
    auto expr = primary();
    
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<CallExprNode> Parser::finishCall(std::unique_ptr<ExprNode> callee) {
    std::vector<std::unique_ptr<ExprNode>> arguments;
    
    auto location = m_previous.location;
    
    // Parse arguments
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after function call arguments");
    
    auto callExpr = std::make_unique<CallExprNode>(std::move(callee), std::move(arguments));
    callExpr->setLocation(location);
    return callExpr;
}

std::unique_ptr<ExprNode> Parser::primary() {
    // Literals
    if (match(TokenType::FALSE)) {
        auto expr = std::make_unique<LiteralExprNode>(false);
        expr->setLocation(m_previous.location);
        return expr;
    }
    
    if (match(TokenType::TRUE)) {
        auto expr = std::make_unique<LiteralExprNode>(true);
        expr->setLocation(m_previous.location);
        return expr;
    }
    
    if (match(TokenType::INTEGER_LITERAL)) {
        // Parse integer literal
        auto expr = std::make_unique<LiteralExprNode>(std::stoll(m_previous.lexeme));
        expr->setLocation(m_previous.location);
        return expr;
    }
    
    if (match(TokenType::FLOAT_LITERAL)) {
        // Parse float literal
        auto expr = std::make_unique<LiteralExprNode>(std::stod(m_previous.lexeme));
        expr->setLocation(m_previous.location);
        return expr;
    }
    
    if (match(TokenType::STRING_LITERAL)) {
        // Remove quotes from string literal
        std::string value = m_previous.lexeme;
        // Remove the surrounding quotes (first and last character)
        value = value.substr(1, value.length() - 2);
        auto expr = std::make_unique<LiteralExprNode>(value);
        expr->setLocation(m_previous.location);
        return expr;
    }
    
    // Variable reference
    if (match(TokenType::IDENTIFIER)) {
        auto expr = std::make_unique<VariableExprNode>(m_previous.lexeme);
        expr->setLocation(m_previous.location);
        return expr;
    }
    
    // Grouping expression
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        auto groupExpr = std::make_unique<GroupingExprNode>(std::move(expr));
        groupExpr->setLocation(m_previous.location);
        return groupExpr;
    }
    
    throw error("Expected expression");
}

// Helper Methods

VariableFlag Parser::parseVariableFlag() {
    if (match(TokenType::STATIC)) return VariableFlag::STATIC;
    if (match(TokenType::VOLATILE)) return VariableFlag::VOLATILE;
    if (match(TokenType::ANCHOR)) return VariableFlag::ANCHOR;
    if (match(TokenType::WEAVER)) return VariableFlag::WEAVER;
    if (match(TokenType::FLUX)) return VariableFlag::FLUX;
    if (match(TokenType::ECHO)) return VariableFlag::ECHO;
    
    throw error("Expected variable flag after '::'");
}

VariableModifier Parser::parseVariableModifier() {
    if (match(TokenType::CONF)) return VariableModifier::CONF;
    if (match(TokenType::REB)) return VariableModifier::REB;
    
    // Default to CONF if no modifier specified
    return VariableModifier::CONF;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    throw error(errorMessage);
}

bool Parser::check(TokenType type) const {
    if (m_current.isType(TokenType::EOF_TOKEN)) return type == TokenType::EOF_TOKEN;
    return m_current.isType(type);
}

bool Parser::matchOne(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::advance() {
    m_previous = m_current;
    m_current = m_lexer->nextToken();
    return m_previous;
}

ChronovyanParseError Parser::error(const std::string& message) {
    return error(m_current, message);
}

ChronovyanParseError Parser::error(const Token& token, const std::string& message) {
    std::stringstream errorMsg;
    errorMsg << token.location.toString() << ": " << message;
    
    // Report the error through the error handler instance
    ErrorHandler::getInstance().reportError(
        token.location,
        message,
        ErrorSeverity::ERROR
    );
    
    m_hadError = true;
    return ChronovyanParseError(errorMsg.str(), token.location);
}

void Parser::synchronize() {
    // If we're already at EOF, nothing to synchronize
    if (m_current.isType(TokenType::EOF_TOKEN)) return;
    
    advance(); // Skip the token that caused the error
    
    // Skip tokens until we find a statement boundary
    while (!m_current.isType(TokenType::EOF_TOKEN)) {
        // Statement boundaries often occur after semicolons
        if (m_previous.isType(TokenType::SEMICOLON)) return;
        
        // Or at the start of a new declaration or statement
        switch (m_current.type) {
            case TokenType::DECLARE:
            case TokenType::IF:
            case TokenType::FOR_CHRONON:
            case TokenType::WHILE_EVENT:
            case TokenType::REWIND_FLOW:
            case TokenType::BRANCH_TIMELINE:
            case TokenType::MERGE_TIMELINES:
            case TokenType::TEMPORAL_ECHO_LOOP:
                return;
            default:
                break;
        }
        
        advance();
    }
    
    // We've reached EOF, nothing more to synchronize
}

} // namespace chronovyan 