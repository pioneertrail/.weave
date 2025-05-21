#pragma once

#include <chrono>
#include <string>

namespace chronovyan {

// Enum for performance modes
enum class PerformanceMode {
    HighFidelity,
    Balanced,
    Lean
};

// Metric source interface
class IMetricSource {
public:
    virtual ~IMetricSource() = default;
    virtual double getValue() const = 0;
    virtual bool isAvailable() const = 0;
    virtual std::chrono::system_clock::time_point getLastUpdateTime() const = 0;
};

} // namespace chronovyan 