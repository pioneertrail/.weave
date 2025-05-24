#pragma once

#include "chronovyan/performance_mode.hpp"
#include "chronovyan/common_types.hpp"
#include <string>
#include <chrono>
#include <vector>

namespace chronovyan {

/**
 * Engine that makes decisions about which performance mode to use
 * based on current system metrics.
 */
class ModeDecisionEngine {
public:
    ModeDecisionEngine() 
        : force_mode_for_testing_(false),
          forced_mode_(PerformanceMode::Balanced),
          forced_reason_("Mode forced for testing"),
          had_previous_source_failure_(false),
          cpu_was_unavailable_(false),
          memory_was_unavailable_(false),
          gpu_was_unavailable_(false) {}
    
    // Make a mode decision based on current metrics
    ModeDecision makeDecision(const SystemMetrics& metrics);
    
    // Set a forced mode for testing
    void setForceModeForTesting(PerformanceMode mode, bool enable, 
                              const std::string& reason = "Mode forced for testing");
    
    // Static method to control forced stability for testing
    static void setForceStableForTesting(bool force_stable);
    static bool getForceStableForTesting();
    
    // Utility for setting source recovery testing state
    void setSourceRecoveryTestingState(bool cpu_unavailable, bool memory_unavailable, bool gpu_unavailable) {
        cpu_was_unavailable_ = cpu_unavailable;
        memory_was_unavailable_ = memory_unavailable;
        gpu_was_unavailable_ = gpu_unavailable;
        had_previous_source_failure_ = cpu_unavailable || memory_unavailable || gpu_unavailable;
    }
    
    // Get last processed metrics (for testing)
    const SystemMetrics& getLastProcessedMetrics() const {
        return last_processed_metrics_;
    }
    
private:
    // Evaluate metrics and make a decision
    ModeDecision evaluate_metrics(const SystemMetrics& metrics);
    
    // Helper methods
    bool is_metric_stale(const std::chrono::system_clock::time_point& timestamp) const;
    bool should_apply_hysteresis(const ModeDecision& new_decision) const;
    ModeDecision make_conservative_decision(const std::string& reason, const std::string& details) const;
    bool is_metric_valid(double value) const;
    
    // Thresholds
    static constexpr double kHighLoadThreshold = 85.0;
    static constexpr double kMediumLoadThreshold = 60.0;
    static constexpr double kLowLoadThreshold = 30.0;
    static constexpr int kStaleMetricThreshold = 5; // seconds
    
    // State for forced mode testing
    bool force_mode_for_testing_;
    PerformanceMode forced_mode_;
    std::string forced_reason_;
    
    // Last decision tracking (for hysteresis)
    ModeDecision last_decision_;
    std::chrono::system_clock::time_point last_decision_time_;
    
    // Track previous source failures for recovery detection
    bool had_previous_source_failure_;
    bool cpu_was_unavailable_;
    bool memory_was_unavailable_;
    bool gpu_was_unavailable_;
    
    // Store last processed metrics for testing
    SystemMetrics last_processed_metrics_;
    
    // Static state for testing
    static bool force_stable_for_testing_;
};

} // namespace chronovyan 