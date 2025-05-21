#pragma once

#include <vector>
#include <functional>
#include "chronovyan/optimization_metrics.hpp"

namespace chronovyan {
namespace sync {

class RealTimeOptimizer {
public:
    RealTimeOptimizer(
        double initial_threshold = 0.5,
        double min_threshold = 0.1,
        double max_threshold = 0.9,
        size_t window_size = 100
    );
    
    void update_metrics(const OptimizationMetrics& metrics);
    double get_current_threshold() const;
    OptimizationMetrics get_performance_metrics() const;
    
private:
    void update_thresholds();
    void calculate_performance_metrics();
    double calculate_moving_average(
        std::function<double(const OptimizationMetrics&)> metric_selector
    ) const;
    double calculate_weighted_moving_average(
        std::function<double(const OptimizationMetrics&)> metric_selector
    ) const;
    void calculate_trend_indicators();
    
    double current_threshold;
    double min_threshold;
    double max_threshold;
    size_t window_size;
    std::vector<OptimizationMetrics> metrics_history;
    OptimizationMetrics performance_metrics;
};

} // namespace sync
} // namespace chronovyan 