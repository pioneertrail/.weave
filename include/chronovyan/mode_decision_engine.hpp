#pragma once

#include "chronovyan/common_types.hpp"
#include <chrono>
#include <string>

namespace chronovyan {

// Forward declarations
class MetricCollector;
struct SystemMetrics;

/**
 * @brief Decision made by the ModeDecisionEngine
 */
struct ModeDecision {
    PerformanceMode mode;              ///< The recommended performance mode
    std::string reason;               ///< The reason for the mode change
    std::string details;              ///< Additional details about the decision
    bool is_error_state{false};       ///< Whether this decision is due to an error state
    bool is_fallback_mode{false};     ///< Whether this is a fallback mode decision
    bool is_conservative{false};      ///< Whether this is a conservative decision
    bool requires_fallback{false};    ///< Whether this decision requires a fallback mode
};

/**
 * @brief Engine responsible for making mode decisions based on system metrics
 */
class ModeDecisionEngine {
public:
    ModeDecisionEngine() : 
        last_decision_time_(std::chrono::system_clock::time_point{}),
        force_mode_for_testing_(PerformanceMode::Balanced),
        use_forced_mode_for_testing_(false) {}
    ~ModeDecisionEngine() = default;

    // Prevent copying
    ModeDecisionEngine(const ModeDecisionEngine&) = delete;
    ModeDecisionEngine& operator=(const ModeDecisionEngine&) = delete;

    /**
     * @brief Evaluates system metrics and makes a mode decision
     * @param metrics The current system metrics
     * @return A ModeDecision containing the recommended mode and decision details
     */
    ModeDecision evaluate_metrics(const SystemMetrics& metrics);

    /**
     * @brief Gets the last decision made by the engine
     * @return The last ModeDecision
     */
    const ModeDecision& get_last_decision() const { return last_decision_; }

    // Test helpers
    void setForceModeForTesting(PerformanceMode mode, bool use_forced_mode = true) {
        force_mode_for_testing_ = mode;
        use_forced_mode_for_testing_ = use_forced_mode;
    }
    
    bool isUsingForcedModeForTesting() const { 
        return use_forced_mode_for_testing_; 
    }
    
    PerformanceMode getForcedModeForTesting() const {
        return force_mode_for_testing_;
    }

private:
    // Constants for decision making
    static constexpr double kHighLoadThreshold = 80.0;      ///< Threshold for high load (percentage)
    static constexpr double kLowLoadThreshold = 30.0;       ///< Threshold for low load (percentage)
    static constexpr double kStaleMetricThreshold = 1.0;    ///< Threshold for stale metrics (seconds)
    
    // Time of last decision for hysteresis
    std::chrono::system_clock::time_point last_decision_time_;
    ModeDecision last_decision_;
    
    // Test helpers
    PerformanceMode force_mode_for_testing_;
    bool use_forced_mode_for_testing_;
    
    // Helper methods
    bool is_metric_stale(const std::chrono::system_clock::time_point& timestamp) const;
    bool should_apply_hysteresis(const ModeDecision& new_decision) const;
    ModeDecision make_conservative_decision(const std::string& reason, const std::string& details = "") const;
    bool is_metric_valid(double value) const;
};

} // namespace chronovyan 