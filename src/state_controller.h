class StateController {
public:
    // ... existing public methods ...

    // Update mode based on decision - now takes a pointer to support null decision test case
    void updateMode(const ModeDecision* decision);

    // ... existing code ...

private:
    // ... existing private methods and members ...

    // Special handlers for specific decision types
    void handleNullDecision();
    void handleErrorState(const ModeDecision& decision, PerformanceMode old_mode, std::chrono::system_clock::time_point timestamp);
    void handleFallbackMode(const ModeDecision& decision, PerformanceMode old_mode, std::chrono::system_clock::time_point timestamp);

    // Record a mode change in history with provided timestamp
    void recordModeChange(const ModeDecision& decision, PerformanceMode old_mode, std::chrono::system_clock::time_point timestamp);

    // ... existing code ...
}; 