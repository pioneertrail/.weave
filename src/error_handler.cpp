#include "error_handler.h"
#include <iostream>

namespace chronovyan {

std::string ChronovyanError::toString() const {
    std::string prefix;
    
    switch (severity) {
        case ErrorSeverity::WARNING:
            prefix = "Warning";
            break;
        case ErrorSeverity::ERROR:
            prefix = "Error";
            break;
        case ErrorSeverity::FATAL:
            prefix = "Fatal error";
            break;
    }
    
    if (location.isValid()) {
        return prefix + " at " + location.toString() + ": " + message;
    } else {
        return prefix + ": " + message;
    }
}

void ErrorHandler::reportError(
    const SourceLocation& location, 
    const std::string& message, 
    ErrorSeverity severity
) {
    m_errors.emplace_back(location, message, severity);
}

void ErrorHandler::reportWarning(
    const SourceLocation& location, 
    const std::string& message
) {
    m_errors.emplace_back(location, message, ErrorSeverity::WARNING);
}

void ErrorHandler::reportFatalError(
    const SourceLocation& location, 
    const std::string& message
) {
    m_errors.emplace_back(location, message, ErrorSeverity::FATAL);
    throw ChronovyanException(message);
}

bool ErrorHandler::hasErrors() const {
    for (const auto& error : m_errors) {
        if (error.severity != ErrorSeverity::WARNING) {
            return true;
        }
    }
    
    return false;
}

const std::vector<ChronovyanError>& ErrorHandler::getErrors() const {
    return m_errors;
}

void ErrorHandler::clearErrors() {
    m_errors.clear();
}

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

} // namespace chronovyan 