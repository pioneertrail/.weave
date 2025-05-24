#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <map>
#include "chronovyan/common_types.hpp"

namespace chronovyan {

// Structure to hold individual metric data
struct MetricData {
    double value = 0.0;
    std::chrono::system_clock::time_point timestamp;
    bool is_available = true;
};

// Structure to hold all system metrics
struct SystemMetrics {
    // Modern approach: use a map of metrics
    std::map<std::string, MetricData> metrics;
    bool is_stale = false;
    bool is_valid = true;
    
    // Exception tracking
    bool has_exception = false;
    std::string exception_source;
    std::string exception_message;
    
    // Legacy fields for compatibility with tests
    double cpu_usage = 0.0;
    double memory_usage = 0.0;
    double gpu_usage = 0.0;
};

class MetricCollector {
public:
    // Constants for default values
    static constexpr double kDefaultCpuUsage = 0.0;
    static constexpr double kDefaultMemoryUsage = 0.0;
    static constexpr double kDefaultGpuUsage = 0.0;
    static constexpr std::chrono::seconds kStaleThreshold = std::chrono::seconds(1);
    
    // Default constructor for testing
    MetricCollector();
    
    // Constructor takes metric sources as dependencies
    MetricCollector(
        IMetricSource* cpu_source,
        IMetricSource* memory_source,
        IMetricSource* gpu_source
    );
    
    // Add a source with a name
    void addSource(const std::string& name, IMetricSource* source);
    
    // Main method to collect all metrics
    SystemMetrics collect_metrics() const;
    
    // Individual metric getters
    double get_cpu_usage() const;
    double get_memory_usage() const;
    double get_gpu_usage() const;
    
    // Testing helper function to force metrics to never be stale
    static void setForceRefreshForTesting(bool force_refresh);
    static bool getForceRefreshForTesting();
    
private:
    // Helper methods
    double collect_metric(IMetricSource* source) const;
    bool is_metric_stale(IMetricSource* source) const;
    double clamp_metric(double value) const;
    
    // Metric sources
    IMetricSource* cpu_source_ = nullptr;
    IMetricSource* memory_source_ = nullptr;
    IMetricSource* gpu_source_ = nullptr;
    
    // Named sources for extensibility
    std::map<std::string, IMetricSource*> sources_;
    
    // Static member to force refresh for testing
    static bool force_refresh_for_testing_;
    
    // Temporary storage for exception information
    mutable std::string last_error_;
};

} // namespace chronovyan 