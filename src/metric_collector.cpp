#include "chronovyan/metric_collector.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath> // For isnan

namespace chronovyan {

MetricCollector::MetricCollector()
    : cpu_source_(nullptr),
      memory_source_(nullptr),
      gpu_source_(nullptr) {
    // Default constructor for testing
}

MetricCollector::MetricCollector(
    IMetricSource* cpu_source,
    IMetricSource* memory_source,
    IMetricSource* gpu_source
) : cpu_source_(cpu_source),
    memory_source_(memory_source),
    gpu_source_(gpu_source) {
    if (!cpu_source_ || !memory_source_ || !gpu_source_) {
        throw std::invalid_argument("Metric sources cannot be null");
    }
    
    // Also add to named sources map
    sources_["cpu"] = cpu_source_;
    sources_["memory"] = memory_source_;
    sources_["gpu"] = gpu_source_;
}

void MetricCollector::addSource(const std::string& name, IMetricSource* source) {
    if (!source) {
        throw std::invalid_argument("Cannot add null source");
    }
    
    sources_[name] = source;
    
    // Set legacy fields if appropriate
    if (name == "cpu") {
        cpu_source_ = source;
    } else if (name == "memory") {
        memory_source_ = source;
    } else if (name == "gpu") {
        gpu_source_ = source;
    }
}

SystemMetrics MetricCollector::collect_metrics() const {
    SystemMetrics metrics;
    metrics.is_valid = true;
    
    // Collect metrics using modern approach
    double cpu_value = collect_metric(cpu_source_);
    double memory_value = collect_metric(memory_source_);
    double gpu_value = collect_metric(gpu_source_);
    
    // Create metric entries
    MetricData cpu_metric = {
        cpu_value,
        cpu_source_->getLastUpdateTime(),
        cpu_source_->isAvailable()
    };
    
    MetricData memory_metric = {
        memory_value,
        memory_source_->getLastUpdateTime(),
        memory_source_->isAvailable()
    };
    
    MetricData gpu_metric = {
        gpu_value,
        gpu_source_->getLastUpdateTime(),
        gpu_source_->isAvailable()
    };
    
    // Add metrics to the map
    metrics.metrics["cpu"] = cpu_metric;
    metrics.metrics["memory"] = memory_metric;
    metrics.metrics["gpu"] = gpu_metric;
    
    // Also set legacy fields for compatibility with tests
    metrics.cpu_usage = cpu_value;
    metrics.memory_usage = memory_value;
    metrics.gpu_usage = gpu_value;
    
    // Check if any metric is stale
    metrics.is_stale = is_metric_stale(cpu_source_) ||
                       is_metric_stale(memory_source_) ||
                       is_metric_stale(gpu_source_);
    
    return metrics;
}

double MetricCollector::get_cpu_usage() const {
    return collect_metric(cpu_source_);
}

double MetricCollector::get_memory_usage() const {
    return collect_metric(memory_source_);
}

double MetricCollector::get_gpu_usage() const {
    return collect_metric(gpu_source_);
}

double MetricCollector::collect_metric(IMetricSource* source) const {
    if (!source->isAvailable()) {
        return kDefaultCpuUsage;  // Use default value if source is unavailable
    }
    
    try {
        double value = source->getValue();
        return clamp_metric(value);
    } catch (const std::exception&) {
        return kDefaultCpuUsage;  // Use default value if source throws
    }
}

bool MetricCollector::is_metric_stale(IMetricSource* source) const {
    if (!source->isAvailable()) {
        return true;
    }
    
    auto last_update = source->getLastUpdateTime();
    auto now = std::chrono::system_clock::now();
    return (now - last_update) > kStaleThreshold;
}

double MetricCollector::clamp_metric(double value) const {
    // Handle NaN
    if (std::isnan(value)) {
        return kDefaultCpuUsage;
    }
    
    // Clamp to [0.0, 100.0]
    return std::clamp(value, 0.0, 100.0);
}

} // namespace chronovyan 