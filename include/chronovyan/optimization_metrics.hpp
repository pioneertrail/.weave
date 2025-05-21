#pragma once

namespace chronovyan {
namespace sync {

// Metrics for system optimization
struct OptimizationMetrics {
    double cpu_usage{0.0};
    double memory_usage{0.0};
    double sync_efficiency{1.0};
    double latency{0.0};
    double error_rate{0.0};
    double stability{1.0};
    double stability_score{1.0};    // Added for compatibility with tests
    double coherence_score{1.0};    // Added for compatibility with tests
    
    // Default constructor
    OptimizationMetrics() = default;
    
    // Constructor used in tests
    OptimizationMetrics(double sync_eff, double stab_score, double coh_score) : 
        sync_efficiency(sync_eff),
        stability(stab_score),      // Map to existing stability field
        stability_score(stab_score),
        coherence_score(coh_score)
    {}
};

// Performance metrics
struct PerformanceMetrics {
    double throughput{0.0};          // Operations per second
    double response_time{0.0};       // Average response time in milliseconds
    double resource_efficiency{0.0}; // Resource efficiency (0.0 - 1.0)
    double accuracy{0.0};            // Accuracy of operations (0.0 - 1.0)
    double cpu_usage{0.0};           // CPU usage percentage (0.0 - 100.0)
    double memory_usage{0.0};        // Memory usage percentage (0.0 - 100.0)
    
    PerformanceMetrics() = default;
};

} // namespace sync
} // namespace chronovyan 