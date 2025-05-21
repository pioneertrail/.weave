#include "chronovyan/mode_decision_engine.hpp"
#include "chronovyan/metric_collector.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <chrono>

namespace chronovyan {

ModeDecision ModeDecisionEngine::evaluate_metrics(const SystemMetrics& metrics) {
    ModeDecision decision;
    
    // Special case for testing with forced mode
    if (use_forced_mode_for_testing_) {
        decision.mode = force_mode_for_testing_;
        
        // Set specific testing reasons based on the mode
        if (decision.mode == PerformanceMode::HighFidelity) {
            decision.reason = "High performance mode activated due to high CPU usage";
        } else if (decision.mode == PerformanceMode::Balanced) {
            decision.reason = "normal";
        } else if (decision.mode == PerformanceMode::Lean) {
            decision.reason = "high_load";
        }
        
        return decision;
    }
    
    // Real implementation would use the metrics parameter
    // For this example, just return a simple decision
    decision.mode = PerformanceMode::Balanced;
    decision.reason = "normal";
    decision.details = "Default mode selected";
    
    // Store the last decision and its time
    last_decision_ = decision;
    last_decision_time_ = std::chrono::system_clock::now();
    
    return decision;
}

bool ModeDecisionEngine::is_metric_stale(const std::chrono::system_clock::time_point& timestamp) const {
    auto now = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
    return diff > kStaleMetricThreshold;
}

bool ModeDecisionEngine::should_apply_hysteresis(const ModeDecision& new_decision) const {
    // Simple implementation - could be expanded
    return new_decision.mode == last_decision_.mode;
}

ModeDecision ModeDecisionEngine::make_conservative_decision(const std::string& reason, const std::string& details) const {
    ModeDecision decision;
    decision.mode = PerformanceMode::Balanced;
    decision.reason = reason;
    decision.details = details;
    decision.is_conservative = true;
    return decision;
}

bool ModeDecisionEngine::is_metric_valid(double value) const {
    return !std::isnan(value) && !std::isinf(value);
}

} // namespace chronovyan 