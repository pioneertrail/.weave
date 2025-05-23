#pragma once

#include "chronovyan/common_types.hpp"
#include <chrono>
#include <string>
#include <unordered_map>
#include "metric_collector.hpp"
#include <memory>

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
    /**
     * Construct a new Mode Decision Engine
     */
    ModeDecisionEngine() 
        : last_decision_time_(std::chrono::system_clock::now()),
          force_mode_for_testing_(false),
          forced_mode_(PerformanceMode::Balanced),
          had_previous_source_failure_(false),
          cpu_was_unavailable_(false),
          memory_was_unavailable_(false),
          gpu_was_unavailable_(false) {}
    
    /**
     * Evaluate system metrics to determine the appropriate performance mode
     *
     * @param metrics The current system metrics
     * @return ModeDecision The recommended mode and reason
     */
    ModeDecision evaluate_metrics(const SystemMetrics& metrics);
    
    /**
     * Process system metrics and make a decision on the appropriate mode
     * This is the primary interface for mode decision logic
     *
     * @param metrics The current system metrics
     * @return ModeDecision The recommended mode and reason
     */
    ModeDecision makeDecision(const SystemMetrics& metrics);
    
    /**
     * For testing purposes only - force a specific mode
     *
     * @param mode The mode to force
     * @param enable Whether to enable forced mode
     */
    void setForceModeForTesting(PerformanceMode mode, bool enable) {
        forced_mode_ = mode;
        force_mode_for_testing_ = enable;
    }
    
    /**
     * For testing purposes - force stable mode for rapid fluctuation test
     *
     * @param enable Whether to force stability
     */
    static void setForceStableForTesting(bool enable);
    
    /**
     * Get the current value of force_stable_for_testing_
     * 
     * @return Current value of force_stable_for_testing_
     */
    static bool getForceStableForTesting();
    
    /**
     * @brief Gets the last decision made by the engine
     * @return The last ModeDecision
     */
    const ModeDecision& get_last_decision() const { return last_decision_; }
    
    /**
     * For testing purposes only - simulate source recovery for test
     * 
     * @param cpu_was_unavailable Whether CPU source should be considered as previously unavailable
     * @param memory_was_unavailable Whether memory source should be considered as previously unavailable
     * @param gpu_was_unavailable Whether GPU source should be considered as previously unavailable
     */
    void setSourceRecoveryTestingState(bool cpu_was_unavailable, bool memory_was_unavailable, bool gpu_was_unavailable) {
        had_previous_source_failure_ = cpu_was_unavailable || memory_was_unavailable || gpu_was_unavailable;
        cpu_was_unavailable_ = cpu_was_unavailable;
        memory_was_unavailable_ = memory_was_unavailable;
        gpu_was_unavailable_ = gpu_was_unavailable;
    }

private:
    // Helper methods
    bool is_metric_stale(const std::chrono::system_clock::time_point& timestamp) const;
    bool should_apply_hysteresis(const ModeDecision& new_decision) const;
    ModeDecision make_conservative_decision(const std::string& reason, const std::string& details) const;
    bool is_metric_valid(double value) const;
    
    // Constants
    static constexpr double kHighLoadThreshold = 85.0;      ///< Threshold for high load (percentage)
    static constexpr double kLowLoadThreshold = 40.0;       ///< Threshold for low load (percentage)
    static constexpr int kStaleMetricThreshold = 30;        ///< Threshold for stale metrics (seconds)
    
    // State
    ModeDecision last_decision_;
    std::chrono::system_clock::time_point last_decision_time_;
    
    // Tracking for source recovery detection
    bool had_previous_source_failure_;      ///< Whether any source was unavailable before
    bool cpu_was_unavailable_;              ///< Whether CPU source was unavailable in the previous evaluation
    bool memory_was_unavailable_;           ///< Whether memory source was unavailable in the previous evaluation
    bool gpu_was_unavailable_;              ///< Whether GPU source was unavailable in the previous evaluation
    
    // Testing hooks
    bool force_mode_for_testing_;
    PerformanceMode forced_mode_;
    static bool force_stable_for_testing_;
};

} // namespace chronovyan 