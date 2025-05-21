#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <map>
#include <vector>

namespace chronovyan {

// Simple struct to store metric data with timestamp
struct MetricData {
    std::string name;
    double value;
    std::chrono::system_clock::time_point timestamp;
    
    MetricData() = default;
    
    MetricData(const std::string& n, double v)
        : name(n), value(v), timestamp(std::chrono::system_clock::now()) {}
    
    MetricData(const std::string& n, double v, std::chrono::system_clock::time_point ts)
        : name(n), value(v), timestamp(ts) {}
};

// Container for system metrics
struct SystemMetrics {
    std::map<std::string, double> values;
    std::chrono::system_clock::time_point timestamp;
    
    SystemMetrics() : timestamp(std::chrono::system_clock::now()) {}
    
    void add(const std::string& name, double value) {
        values[name] = value;
    }
    
    double get(const std::string& name, double default_value = 0.0) const {
        auto it = values.find(name);
        return (it != values.end()) ? it->second : default_value;
    }
};

// Interface for any metric source
class IMetricSource {
public:
    virtual ~IMetricSource() = default;
    
    // Get the name of this metric source
    virtual std::string get_source_name() const = 0;
    
    // Collect metrics from this source
    virtual SystemMetrics collect_metrics() = 0;
    
    // Get a list of metric names provided by this source
    virtual std::vector<std::string> get_metric_names() const = 0;
    
    // Check if this source provides a specific metric
    virtual bool provides_metric(const std::string& metric_name) const = 0;
};

// Factory function to create a metric source
std::unique_ptr<IMetricSource> create_metric_source(const std::string& source_type);

} // namespace chronovyan 