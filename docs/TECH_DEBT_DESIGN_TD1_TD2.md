# Technical Debt Design Document: TD-1 and TD-2

This document outlines the design approach for addressing the high-priority technical debt items TD-1 (Complex error handling in StateController) and TD-2 (Duplicate recovery logic in ModeDecisionEngine).

## TD-1: Complex Error Handling in StateController

### Current Issues

1. Error handling logic is scattered throughout the StateController class
2. Duplication exists between handleErrorState and handleFallbackMode methods
3. String-based error reasons make categorization and filtering difficult
4. Error notification logic is mixed with state management

### Proposed Solution: Polymorphic Error Type System

#### 1. Create Error Type Hierarchy

```cpp
// Base error class
class ChronovyanError {
public:
    virtual ~ChronovyanError() = default;
    virtual std::string getErrorCode() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual ErrorSeverity getSeverity() const = 0;
    virtual bool requiresFallback() const = 0;
};

// Derived error classes
class SensorFailureError : public ChronovyanError {
    // Implementation specific to sensor failures
};

class CommunicationError : public ChronovyanError {
    // Implementation specific to communication errors
};

class ConfigurationError : public ChronovyanError {
    // Implementation specific to configuration errors
};

// Factory for creating error objects
class ErrorFactory {
public:
    static std::unique_ptr<ChronovyanError> createError(
        const std::string& source,
        const std::string& message,
        bool is_critical);
};
```

#### 2. Refactor StateController Error Handling

```cpp
// New method to handle all errors
void StateController::handleError(const std::unique_ptr<ChronovyanError>& error) {
    // Common error handling logic
    
    // Determine if fallback mode is required
    if (error->requiresFallback()) {
        activateFallbackMode(error->getErrorCode());
    } else {
        // Regular error handling
    }
    
    // Notification based on error type
    notification_service_->notifyError(error->getErrorMessage());
}
```

#### 3. Update Decision Engine to Use Error Types

```cpp
ModeDecision ModeDecisionEngine::makeDecision(const SystemMetrics& metrics) {
    try {
        // Existing logic
    } catch (const std::exception& e) {
        auto error = ErrorFactory::createError("engine", e.what(), false);
        return createErrorDecision(error.get());
    }
}

ModeDecision ModeDecisionEngine::createErrorDecision(const ChronovyanError* error) {
    ModeDecision decision;
    decision.mode = PerformanceMode::Lean; // Default conservative mode
    decision.reason = error->getErrorCode();
    decision.details = error->getErrorMessage();
    decision.is_error_state = true;
    decision.is_conservative = true;
    decision.is_fallback_mode = error->requiresFallback();
    decision.requires_fallback = error->requiresFallback();
    return decision;
}
```

### Implementation Strategy

1. Create the error type hierarchy without changing existing code
2. Add factory methods for creating appropriate error types
3. Update StateController to use the new error types while maintaining backward compatibility
4. Update ModeDecisionEngine to produce error-typed decisions
5. Remove the old string-based error handling

### Testing Strategy

1. Create unit tests for each error type
2. Test error creation through the factory
3. Test error handling in StateController with various error types
4. Ensure backward compatibility during the transition

## TD-2: Duplicate Recovery Logic in ModeDecisionEngine

### Current Issues

1. Recovery detection logic is duplicated in evaluate_metrics and makeDecision methods
2. State tracking for sensor availability is scattered
3. Recovery handling is mixed with other decision-making logic
4. Difficult to extend with new recovery scenarios

### Proposed Solution: SensorRecoveryHandler Class

#### 1. Create SensorRecoveryHandler Class

```cpp
class SensorRecoveryHandler {
public:
    SensorRecoveryHandler();
    
    // Track sensor availability
    void updateSensorState(const std::string& sensor_name, bool is_available);
    
    // Check for recovery conditions
    bool hasAnySensorRecovered() const;
    bool hasSensorRecovered(const std::string& sensor_name) const;
    
    // Get recovery information
    std::vector<std::string> getRecoveredSensors() const;
    
    // Create recovery decision
    ModeDecision createRecoveryDecision() const;
    
private:
    struct SensorState {
        bool was_unavailable = false;
        bool is_available = true;
    };
    
    std::unordered_map<std::string, SensorState> sensor_states_;
    bool had_previous_failure_ = false;
};
```

#### 2. Integrate with ModeDecisionEngine

```cpp
class ModeDecisionEngine {
private:
    // Add as member variable
    SensorRecoveryHandler recovery_handler_;
    
    // Other existing members
};

ModeDecision ModeDecisionEngine::evaluate_metrics(const SystemMetrics& metrics) {
    // Update recovery handler with current sensor states
    recovery_handler_.updateSensorState("cpu", 
        metrics.metrics.count("cpu") && metrics.metrics.at("cpu").is_available);
    recovery_handler_.updateSensorState("memory", 
        metrics.metrics.count("memory") && metrics.metrics.at("memory").is_available);
    recovery_handler_.updateSensorState("gpu", 
        metrics.metrics.count("gpu") && metrics.metrics.at("gpu").is_available);
    
    // Check for recovery first
    if (recovery_handler_.hasAnySensorRecovered()) {
        return recovery_handler_.createRecoveryDecision();
    }
    
    // Continue with existing logic for non-recovery scenarios
}
```

### Implementation Strategy

1. Create the SensorRecoveryHandler class with tests
2. Integrate it with ModeDecisionEngine without removing existing logic
3. Refactor evaluate_metrics and makeDecision to use the handler
4. Remove duplicate recovery logic once integration is complete

### Testing Strategy

1. Unit tests for SensorRecoveryHandler class
2. Integration tests for ModeDecisionEngine with the handler
3. Regression tests to ensure behavior remains the same

## Combined Implementation Timeline

### Week 1: Error Type System (TD-1)
- Day 1-2: Create error type hierarchy and factory
- Day 3: Update StateController to use new error types
- Day 4-5: Update ModeDecisionEngine and tests

### Week 2: Recovery Handler (TD-2)
- Day 1-2: Implement SensorRecoveryHandler
- Day 3: Integrate with ModeDecisionEngine
- Day 4-5: Clean up duplicate code and testing

## Success Metrics

We will measure the success of these refactorings using:

1. **Code metrics**:
   - Reduction in cyclomatic complexity
   - Reduction in lines of code for error handling
   - Improved code duplication metrics

2. **Quality metrics**:
   - Maintain or improve test coverage
   - Reduce the number of error-related bugs

3. **Developer experience**:
   - Ease of adding new error types
   - Simplicity of recovery logic

## Risks and Mitigations

| Risk | Mitigation |
|------|------------|
| Behavioral changes during refactoring | Comprehensive test suite with before/after validation |
| Performance impact of polymorphism | Benchmark critical paths before and after changes |
| Learning curve for new abstractions | Document with examples and provide team training | 