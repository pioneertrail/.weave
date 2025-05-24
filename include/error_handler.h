#ifndef CHRONOVYAN_ERROR_HANDLER_H
#define CHRONOVYAN_ERROR_HANDLER_H

#include "source_location.h"
#include <string>
#include <vector>
#include <stdexcept>

namespace chronovyan {

/**
 * @enum ErrorSeverity
 * @brief Defines the severity levels for errors
 */
enum class ErrorSeverity {
    WARNING,    // Non-fatal issues that don't prevent execution
    ERROR,      // Problems that prevent execution but allow recovery
    FATAL       // Unrecoverable errors
};

/**
 * @struct ChronovyanError
 * @brief Represents an error in Chronovyan code
 */
struct ChronovyanError {
    SourceLocation location;
    std::string message;
    ErrorSeverity severity;

    ChronovyanError(
        SourceLocation loc,
        std::string msg,
        ErrorSeverity sev = ErrorSeverity::ERROR
    ) : location(std::move(loc)), message(std::move(msg)), severity(sev) {}

    /**
     * @brief Get a formatted string representation of this error
     */
    std::string toString() const;
};

/**
 * @class ErrorHandler
 * @brief Manages errors and warnings during interpretation
 */
class ErrorHandler {
public:
    /**
     * @brief Add an error to the collection
     */
    void reportError(const SourceLocation& location, 
                    const std::string& message, 
                    ErrorSeverity severity = ErrorSeverity::ERROR);

    /**
     * @brief Add a warning to the collection
     */
    void reportWarning(const SourceLocation& location, const std::string& message);

    /**
     * @brief Add a fatal error and throw an exception
     * @throws ChronovyanFatalError
     */
    [[noreturn]] void reportFatalError(const SourceLocation& location, const std::string& message);

    /**
     * @brief Check if any errors (excluding warnings) have been reported
     */
    bool hasErrors() const;

    /**
     * @brief Get all reported errors
     */
    const std::vector<ChronovyanError>& getErrors() const;

    /**
     * @brief Clear all errors
     */
    void clearErrors();

    /**
     * @brief Get a singleton instance of the error handler
     */
    static ErrorHandler& getInstance();

private:
    // Private constructor for singleton pattern
    ErrorHandler() = default;

    std::vector<ChronovyanError> m_errors;
};

/**
 * @class ChronovyanException
 * @brief Base class for all Chronovyan exceptions
 */
class ChronovyanException : public std::runtime_error {
public:
    ChronovyanException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class ChronovyanParseError
 * @brief Exception for parsing errors
 */
class ChronovyanParseError : public ChronovyanException {
public:
    ChronovyanParseError(const std::string& message, const SourceLocation& location)
        : ChronovyanException(message), m_location(location) {}

    const SourceLocation& getLocation() const { return m_location; }

private:
    SourceLocation m_location;
};

/**
 * @class ChronovyanRuntimeError
 * @brief Exception for runtime errors
 */
class ChronovyanRuntimeError : public ChronovyanException {
public:
    ChronovyanRuntimeError(const std::string& message, const SourceLocation& location)
        : ChronovyanException(message), m_location(location) {}

    const SourceLocation& getLocation() const { return m_location; }

private:
    SourceLocation m_location;
};

} // namespace chronovyan

#endif // CHRONOVYAN_ERROR_HANDLER_H 