#include "chronovyan/real_time_optimizer.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace chronovyan {
namespace sync {

RealTimeOptimizer::RealTimeOptimizer(
    double initial_threshold,
    double min_threshold,
    double max_threshold,
    size_t window_size
) : current_threshold(initial_threshold),
    min_threshold(min_threshold),
    max_threshold(max_threshold),
    window_size(window_size) {
    metrics_history.reserve(window_size);
}

void RealTimeOptimizer::update_metrics(const OptimizationMetrics& metrics) {
    metrics_history.push_back(metrics);
    if (metrics_history.size() > window_size) {
        metrics_history.erase(metrics_history.begin());
    }
    
    update_thresholds();
    calculate_performance_metrics();
}

double RealTimeOptimizer::get_current_threshold() const {
    return current_threshold;
}

OptimizationMetrics RealTimeOptimizer::get_performance_metrics() const {
    return performance_metrics;
}

void RealTimeOptimizer::update_thresholds() {
    if (metrics_history.empty()) {
        return;
    }
    
    // Calculate moving averages
    double avg_efficiency = calculate_moving_average(
        [](const OptimizationMetrics& m) { return m.sync_efficiency; }
    );
    double avg_stability = calculate_moving_average(
        [](const OptimizationMetrics& m) { return m.stability; }
    );
    double avg_coherence = calculate_moving_average(
        [](const OptimizationMetrics& m) { return 1.0 - m.error_rate / 100.0; }
    );
    
    // Calculate threshold adjustment factors
    double efficiency_factor = (avg_efficiency - 0.5) * 2.0; // Scale to [-1, 1]
    double stability_factor = (avg_stability - 0.5) * 2.0;
    double coherence_factor = (avg_coherence - 0.5) * 2.0;
    
    // Combine factors with weights
    double adjustment_factor = 
        0.4 * efficiency_factor +
        0.3 * stability_factor +
        0.3 * coherence_factor;
    
    // Apply adaptive threshold adjustment
    double threshold_change = adjustment_factor * 0.1; // Scale adjustment
    current_threshold = std::clamp(
        current_threshold + threshold_change,
        min_threshold,
        max_threshold
    );
}

void RealTimeOptimizer::calculate_performance_metrics() {
    if (metrics_history.empty()) {
        return;
    }
    
    // Calculate weighted moving averages
    performance_metrics.sync_efficiency = calculate_weighted_moving_average(
        [](const OptimizationMetrics& m) { return m.sync_efficiency; }
    );
    performance_metrics.stability = calculate_weighted_moving_average(
        [](const OptimizationMetrics& m) { return m.stability; }
    );
    // Calculate error rate (which will be used as inverse of coherence)
    performance_metrics.error_rate = calculate_weighted_moving_average(
        [](const OptimizationMetrics& m) { return m.error_rate; }
    );
    
    // Calculate trend indicators
    calculate_trend_indicators();
}

double RealTimeOptimizer::calculate_moving_average(
    std::function<double(const OptimizationMetrics&)> metric_selector
) const {
    if (metrics_history.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (const auto& metrics : metrics_history) {
        sum += metric_selector(metrics);
    }
    return sum / metrics_history.size();
}

double RealTimeOptimizer::calculate_weighted_moving_average(
    std::function<double(const OptimizationMetrics&)> metric_selector
) const {
    if (metrics_history.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    double weight_sum = 0.0;
    
    for (size_t i = 0; i < metrics_history.size(); ++i) {
        double weight = std::exp(-0.1 * (metrics_history.size() - i - 1));
        sum += weight * metric_selector(metrics_history[i]);
        weight_sum += weight;
    }
    
    return sum / weight_sum;
}

void RealTimeOptimizer::calculate_trend_indicators() {
    if (metrics_history.size() < 2) {
        return;
    }
    
    // Calculate exponential moving averages for trend detection
    const double alpha = 0.3; // Smoothing factor
    
    double ema_efficiency = metrics_history.back().sync_efficiency;
    double ema_stability = metrics_history.back().stability;
    double ema_error = metrics_history.back().error_rate;
    
    for (auto it = metrics_history.rbegin() + 1; it != metrics_history.rend(); ++it) {
        ema_efficiency = alpha * it->sync_efficiency + (1 - alpha) * ema_efficiency;
        ema_stability = alpha * it->stability + (1 - alpha) * ema_stability;
        ema_error = alpha * it->error_rate + (1 - alpha) * ema_error;
    }
    
    // Update performance metrics with trend information
    performance_metrics.sync_efficiency = ema_efficiency;
    performance_metrics.stability = ema_stability;
    performance_metrics.error_rate = ema_error;
}

} // namespace sync
} // namespace chronovyan 