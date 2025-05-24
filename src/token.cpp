#include "token.h"
#include <unordered_map>
#include <sstream>

namespace chronovyan {

std::string Token::typeToString() const {
    switch (type) {
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::ERROR: return "ERROR";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL: return "INTEGER";
        case TokenType::FLOAT_LITERAL: return "FLOAT";
        case TokenType::STRING_LITERAL: return "STRING";
        
        // Keywords
        case TokenType::DECLARE: return "DECLARE";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FOR_CHRONON: return "FOR_CHRONON";
        case TokenType::WHILE_EVENT: return "WHILE_EVENT";
        case TokenType::REWIND_FLOW: return "REWIND_FLOW";
        case TokenType::BRANCH_TIMELINE: return "BRANCH_TIMELINE";
        case TokenType::MERGE_TIMELINES: return "MERGE_TIMELINES";
        case TokenType::TEMPORAL_ECHO_LOOP: return "TEMPORAL_ECHO_LOOP";
        case TokenType::PARADOX_HANDLER: return "PARADOX_HANDLER";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::COLLAPSE: return "COLLAPSE";
        
        // Variable modifiers
        case TokenType::CONF: return "CONF";
        case TokenType::REB: return "REB";
        
        // Variable flags
        case TokenType::STATIC: return "STATIC";
        case TokenType::VOLATILE: return "VOLATILE";
        case TokenType::ANCHOR: return "ANCHOR";
        case TokenType::WEAVER: return "WEAVER";
        case TokenType::FLUX: return "FLUX";
        case TokenType::ECHO: return "ECHO";
        
        // Types
        case TokenType::INT: return "INT";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::STRING: return "STRING";
        case TokenType::VOID: return "VOID";
        case TokenType::ARRAY: return "ARRAY";
        case TokenType::MAP: return "MAP";
        case TokenType::TUPLE: return "TUPLE";
        case TokenType::TIMESTAMP: return "TIMESTAMP";
        case TokenType::QUANTUM_STATE: return "QUANTUM_STATE";
        
        // Operators
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::STAR: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::PERCENT: return "%";
        
        // Comparison operators
        case TokenType::EQUAL_EQUAL: return "==";
        case TokenType::BANG_EQUAL: return "!=";
        case TokenType::LESS: return "<";
        case TokenType::LESS_EQUAL: return "<=";
        case TokenType::GREATER: return ">";
        case TokenType::GREATER_EQUAL: return ">=";
        
        // Assignment operators
        case TokenType::EQUAL: return "=";
        case TokenType::PLUS_EQUAL: return "+=";
        case TokenType::MINUS_EQUAL: return "-=";
        case TokenType::STAR_EQUAL: return "*=";
        case TokenType::SLASH_EQUAL: return "/=";
        case TokenType::PERCENT_EQUAL: return "%=";
        
        // Logical operators
        case TokenType::AND: return "&&";
        case TokenType::OR: return "||";
        case TokenType::BANG: return "!";
        
        // Delimiters
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        case TokenType::COLON: return ":";
        case TokenType::SEMICOLON: return ";";
        case TokenType::DOUBLE_COLON: return "::";
        
        // Temporal operators
        case TokenType::TIMELINE_BRANCH: return "->";
        case TokenType::TIMELINE_MERGE: return "<-";
        case TokenType::TEMPORAL_QUERY: return "?:";
        
        default: return "UNKNOWN";
    }
}

std::string Token::toString() const {
    std::stringstream ss;
    ss << typeToString() << " '" << lexeme << "'";
    
    if (location.isValid()) {
        ss << " at " << location.toString();
    }
    
    return ss.str();
}

const std::unordered_map<std::string, TokenType>& getKeywordMap() {
    static const std::unordered_map<std::string, TokenType> keywordMap = {
        {"DECLARE", TokenType::DECLARE},
        {"IF", TokenType::IF},
        {"ELSE", TokenType::ELSE},
        {"FOR_CHRONON", TokenType::FOR_CHRONON},
        {"WHILE_EVENT", TokenType::WHILE_EVENT},
        {"REWIND_FLOW", TokenType::REWIND_FLOW},
        {"BRANCH_TIMELINE", TokenType::BRANCH_TIMELINE},
        {"MERGE_TIMELINES", TokenType::MERGE_TIMELINES},
        {"TEMPORAL_ECHO_LOOP", TokenType::TEMPORAL_ECHO_LOOP},
        {"PARADOX_HANDLER", TokenType::PARADOX_HANDLER},
        {"TRUE", TokenType::TRUE},
        {"FALSE", TokenType::FALSE},
        {"COLLAPSE", TokenType::COLLAPSE},
        
        // Variable modifiers
        {"CONF", TokenType::CONF},
        {"REB", TokenType::REB},
        
        // Variable flags
        {"STATIC", TokenType::STATIC},
        {"VOLATILE", TokenType::VOLATILE},
        {"ANCHOR", TokenType::ANCHOR},
        {"WEAVER", TokenType::WEAVER},
        {"FLUX", TokenType::FLUX},
        {"ECHO", TokenType::ECHO},
        
        // Types
        {"INT", TokenType::INT},
        {"FLOAT", TokenType::FLOAT},
        {"BOOLEAN", TokenType::BOOLEAN},
        {"STRING", TokenType::STRING},
        {"VOID", TokenType::VOID},
        {"ARRAY", TokenType::ARRAY},
        {"MAP", TokenType::MAP},
        {"TUPLE", TokenType::TUPLE},
        {"TIMESTAMP", TokenType::TIMESTAMP},
        {"QUANTUM_STATE", TokenType::QUANTUM_STATE}
    };
    
    return keywordMap;
}

} // namespace chronovyan 