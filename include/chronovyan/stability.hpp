#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include "chronovyan/quantum.hpp"

namespace chronovyan {
namespace stability {

// Stability measurement types
enum class StabilityMetricType {
    Temporal,
    Resource,
    Performance,
    Error,
    Custom
};

// Basic stability metrics
struct StabilityMetrics {
    double temporal_stability{1.0};   // Measure of timing stability (0.0 - 1.0)
    double resource_stability{1.0};   // Measure of resource usage stability (0.0 - 1.0)
    double performance_stability{1.0}; // Measure of performance stability (0.0 - 1.0)
    double error_stability{1.0};      // Measure of error rate stability (0.0 - 1.0)
    double overall_stability{1.0};    // Overall stability score (0.0 - 1.0)
    
    StabilityMetrics() = default;
};

// Stability threshold configuration
struct StabilityThresholds {
    double critical_threshold{0.3};    // Below this requires immediate action
    double warning_threshold{0.6};     // Below this triggers warnings
    double good_threshold{0.8};        // Above this is considered good
    double excellent_threshold{0.95};  // Above this is considered excellent
    
    StabilityThresholds() = default;
};

// Stability event
struct StabilityEvent {
    std::chrono::system_clock::time_point timestamp;
    StabilityMetricType type;
    double value;
    std::string description;
    
    StabilityEvent() = default;
    StabilityEvent(std::chrono::system_clock::time_point ts, 
                   StabilityMetricType t, 
                   double v, 
                   const std::string& desc)
        : timestamp(ts), type(t), value(v), description(desc) {}
};

// Stability Analyzer interface
class IStabilityAnalyzer {
public:
    virtual ~IStabilityAnalyzer() = default;
    
    // Analyze stability based on recent metrics and events
    virtual StabilityMetrics analyze_stability() const = 0;
    
    // Add a stability event for analysis
    virtual void add_event(const StabilityEvent& event) = 0;
    
    // Set stability thresholds
    virtual void set_thresholds(const StabilityThresholds& thresholds) = 0;
    
    // Get current stability thresholds
    virtual StabilityThresholds get_thresholds() const = 0;
    
    // Clear all events
    virtual void clear_events() = 0;
};

// Callback type for stability notifications
using StabilityCallback = std::function<void(const StabilityEvent&)>;

} // namespace stability
} // namespace chronovyan 