#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <queue>
#include <deque>
#include <unordered_map>
#include <numeric>
#include <cmath>
#include <thread>
#include "optimization_metrics.hpp"

namespace chronovyan {
namespace sync {

// Forward declarations
class MLModel;
class RealTimeOptimizer;
class PatternRecognizer;
class ErrorPredictor;
class PerformanceOptimizer;

class TemporalSynchronizer {
public:
    // Internal definitions (define these first)
    struct SyncPoint {
        std::vector<double> primary_points;
        std::vector<double> secondary_points;
        std::vector<double> tertiary_points;
        double stability = 1.0;
        double coherence = 1.0;
        std::vector<double> historical_stability;
        std::vector<double> historical_coherence;
    };
    
    struct SyncPattern {
        std::vector<double> primary_patterns;
        std::vector<double> secondary_patterns;
        std::vector<double> tertiary_patterns;
        double stability = 1.0;
        double coherence = 1.0;
        std::vector<double> pattern_history;
        std::vector<double> stability_history;
    };
    
    struct SyncMetrics {
        std::vector<double> sync_levels;
        std::vector<double> stability_levels;
        std::vector<double> coherence_levels;
        double overall_sync = 1.0;
        double overall_stability = 1.0;
        double overall_coherence = 1.0;
    };

    TemporalSynchronizer();
    ~TemporalSynchronizer();
    void synchronize_temporal_flows();

    // Public methods for testing
    double get_overall_sync() const { return sync_metrics.overall_sync; }
    double get_overall_stability() const { return sync_metrics.overall_stability; }
    double get_overall_coherence() const { return sync_metrics.overall_coherence; }
    
    // New synchronization features
    void set_sync_threshold(double threshold) { 
        std::lock_guard<std::mutex> lock(sync_mutex);
        sync_threshold = std::clamp(threshold, 0.0, 1.0); 
    }
    
    void set_stability_threshold(double threshold) { 
        std::lock_guard<std::mutex> lock(sync_mutex);
        stability_threshold = std::clamp(threshold, 0.0, 1.0); 
    }
    
    void set_coherence_threshold(double threshold) { 
        std::lock_guard<std::mutex> lock(sync_mutex);
        coherence_threshold = std::clamp(threshold, 0.0, 1.0); 
    }
    
    void set_history_size(size_t size) {
        if (size < 1) throw std::invalid_argument("History size must be at least 1");
        history_size = size;
        resize_histories();
    }
    
    // Advanced configuration
    struct SyncConfig {
        double sync_threshold = 0.8;
        double stability_threshold = 0.8;
        double coherence_threshold = 0.8;
        size_t history_size = 10;
        bool enable_auto_recovery = true;
        bool enable_performance_tracking = true;
        std::chrono::milliseconds recovery_timeout{1000};
    };
    
    void configure(const SyncConfig& config) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        sync_threshold = std::clamp(config.sync_threshold, 0.0, 1.0);
        stability_threshold = std::clamp(config.stability_threshold, 0.0, 1.0);
        coherence_threshold = std::clamp(config.coherence_threshold, 0.0, 1.0);
        set_history_size(config.history_size);
        enable_auto_recovery = config.enable_auto_recovery;
        enable_performance_tracking = config.enable_performance_tracking;
        recovery_timeout = config.recovery_timeout;
    }
    
    // Advanced state management
    struct SyncState {
        SyncPoint sync_point;
        SyncPattern sync_pattern;
        SyncMetrics sync_metrics;
        std::chrono::system_clock::time_point timestamp;
        
        SyncState(const SyncPoint& sp, const SyncPattern& pat, const SyncMetrics& met)
            : sync_point(sp), sync_pattern(pat), sync_metrics(met),
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    void save_state() {
        std::lock_guard<std::mutex> lock(sync_mutex);
        last_good_state = std::make_unique<SyncState>(sync_point, sync_pattern, sync_metrics);
    }
    
    void restore_state() {
        std::lock_guard<std::mutex> lock(sync_mutex);
        if (last_good_state) {
            sync_point = last_good_state->sync_point;
            sync_pattern = last_good_state->sync_pattern;
            sync_metrics = last_good_state->sync_metrics;
        }
    }
    
    // Advanced performance monitoring
    struct PerformanceMetrics {
        std::chrono::microseconds last_sync_duration;
        size_t total_sync_operations{0};
        size_t error_count{0};
        double average_sync_time{0.0};
        std::chrono::system_clock::time_point last_error_time;
        std::vector<std::chrono::microseconds> recent_durations;
        double sync_success_rate{1.0};
        double response_time{0.0};
        double resource_efficiency{1.0};
        double cpu_usage{0.0};
        double memory_usage{0.0};
        double accuracy{1.0};
    };
    
    PerformanceMetrics get_performance_metrics() const {
        std::lock_guard<std::mutex> lock(sync_mutex);
        return performance_metrics;
    }
    
    // Advanced error handling
    struct ErrorInfo {
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        double sync_level{0.0};
        double stability_level{0.0};
        double coherence_level{0.0};
    };
    
    void set_error_handler(std::function<void(const ErrorInfo&)> handler) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        error_handler = std::move(handler);
    }
    
    // Advanced recovery strategies
    enum class RecoveryStrategy {
        Automatic,
        Manual,
        Custom
    };
    
    void set_recovery_strategy(RecoveryStrategy strategy) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        recovery_strategy = strategy;
    }
    
    void set_custom_recovery_strategy(std::function<void()> strategy) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        custom_recovery_strategy = std::move(strategy);
    }
    
    // Advanced synchronization control
    void pause_synchronization() {
        std::lock_guard<std::mutex> lock(sync_mutex);
        is_synchronization_paused = true;
    }
    
    void resume_synchronization() {
        std::lock_guard<std::mutex> lock(sync_mutex);
        is_synchronization_paused = false;
    }
    
    bool is_paused() const {
        std::lock_guard<std::mutex> lock(sync_mutex);
        return is_synchronization_paused;
    }
    
    // Advanced monitoring
    void set_sync_callback(std::function<void(double)> callback) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        sync_callback = std::move(callback);
    }
    
    void set_error_callback(std::function<void(const std::exception&)> callback) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        error_callback = std::move(callback);
    }
    
    void set_recovery_callback(std::function<void(bool)> callback) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        recovery_callback = std::move(callback);
    }
    
    // Existing methods...
    std::vector<double> get_sync_points() const;
    std::vector<double> get_sync_patterns() const;
    std::vector<double> get_stability_metrics() const;
    std::vector<double> get_coherence_metrics() const;
    std::vector<double> get_sync_history() const;
    
    void force_error_state();
    void set_minimum_values();
    void set_maximum_values();

    // Advanced pattern recognition
    struct PatternAnalysis {
        double confidence{0.0};
        std::vector<double> pattern_signature;
        std::string pattern_type;
    };
    
    PatternAnalysis analyze_current_pattern() const;
    
    struct ErrorPrediction {
        double probability{0.0};
        std::string predicted_error_type;
        std::chrono::system_clock::time_point predicted_time;
        std::vector<double> contributing_factors;
    };
    
    ErrorPrediction predict_next_error() const;
    
    struct StateAnalysis {
        double health_score{0.0};
        std::vector<std::string> potential_issues;
        std::vector<std::string> recommendations;
        std::chrono::system_clock::time_point analysis_time;
    };
    
    StateAnalysis analyze_current_state() const;
    bool validate_configuration(const SyncConfig& config) const;
    
    struct PatternMetrics {
        double stability_score{0.0};
        double coherence_score{0.0};
        double complexity_score{0.0};
        std::vector<double> pattern_weights;
        std::chrono::system_clock::time_point analysis_time;
    };
    
    PatternMetrics analyze_pattern_metrics() const;
    
    struct OptimizationStrategy {
        double target_efficiency{0.9};
        std::vector<double> parameter_weights;
        std::chrono::milliseconds optimization_interval{1000};
        bool enable_adaptive_optimization{true};
    };
    
    void set_optimization_strategy(const OptimizationStrategy& strategy);
    
    struct HealthMetrics {
        double system_health{0.0};
        double component_health{0.0};
        double resource_health{0.0};
        std::vector<std::string> health_indicators;
        std::chrono::system_clock::time_point measurement_time;
    };
    
    HealthMetrics get_health_metrics() const;
    
    struct PatternMatch {
        double similarity_score{0.0};
        std::string matched_pattern;
        std::vector<double> match_confidence;
        std::chrono::system_clock::time_point match_time;
    };
    
    PatternMatch find_similar_pattern(const std::vector<double>& pattern) const;
    
    struct ErrorAnalysis {
        double severity_score{0.0};
        std::string root_cause;
        std::vector<std::string> impact_areas;
        std::chrono::system_clock::time_point analysis_time;
    };
    
    ErrorAnalysis analyze_error(const ErrorInfo& error) const;
    
    struct PatternPrediction {
        std::vector<double> predicted_values;
        double confidence{0.0};
        std::chrono::system_clock::time_point prediction_time;
        std::vector<std::string> influencing_factors;
    };
    
    PatternPrediction predict_next_pattern() const;
    
    struct AdaptiveOptimizationConfig {
        double learning_rate{0.01};
        double exploration_rate{0.1};
        size_t optimization_window{100};
        bool enable_reinforcement_learning{true};
        std::vector<double> optimization_weights;
    };
    
    void set_adaptive_optimization(const AdaptiveOptimizationConfig& config);
    
    struct PatternCluster {
        std::vector<std::vector<double>> patterns;
        std::vector<double> centroid;
        double cluster_quality{0.0};
        size_t pattern_count{0};
    };
    
    std::vector<PatternCluster> cluster_patterns() const;
    
    struct AnomalyDetection {
        bool is_anomaly{false};
        double anomaly_score{0.0};
        std::string anomaly_type;
        std::vector<std::string> contributing_factors;
        std::chrono::system_clock::time_point detection_time;
    };
    
    AnomalyDetection detect_anomalies() const;
    
    struct PerformanceProfile {
        std::vector<double> cpu_usage_history;
        std::vector<double> memory_usage_history;
        std::vector<double> sync_efficiency_history;
        std::vector<std::chrono::microseconds> operation_durations;
        std::unordered_map<std::string, double> component_metrics;
    };
    
    PerformanceProfile get_performance_profile() const;
    
    struct MLConfig {
        bool enable_ml_optimization = true;
        double learning_rate = 0.01;
        size_t training_window = 1000;
        std::vector<std::string> feature_columns;
        std::string model_type = "gradient_boost";
        bool enable_auto_tuning = true;
    };
    
    void configure_ml(const MLConfig& config);
    
    struct RealTimeConfig {
        bool enable_real_time_optimization = true;
        std::chrono::milliseconds optimization_interval{100};
        double target_latency = 50.0; // ms
        size_t optimization_window = 100;
        bool enable_adaptive_thresholds = true;
    };
    
    void configure_real_time(const RealTimeConfig& config);
    
    struct PatternRecognitionConfig {
        bool enable_deep_learning = true;
        size_t pattern_window = 50;
        double similarity_threshold = 0.8;
        bool enable_pattern_evolution = true;
        std::vector<std::string> pattern_types;
    };
    
    void configure_pattern_recognition(const PatternRecognitionConfig& config);
    
    struct ErrorPredictionConfig {
        bool enable_error_prediction = true;
        size_t prediction_window = 100;
        double confidence_threshold = 0.8;
        bool enable_root_cause_analysis = true;
        std::vector<std::string> error_types;
    };
    
    void configure_error_prediction(const ErrorPredictionConfig& config);
    
    struct PerformanceOptimizationConfig {
        bool enable_performance_optimization = true;
        std::chrono::milliseconds sampling_interval{100};
        size_t optimization_window = 100;
        bool enable_resource_optimization = true;
        std::vector<std::string> optimization_targets;
    };
    
    void configure_performance_optimization(const PerformanceOptimizationConfig& config);
    
private:
    // Internal state variables
    SyncPoint sync_point;
    SyncPattern sync_pattern;
    SyncMetrics sync_metrics;
    
    // Synchronization
    mutable std::mutex sync_mutex;
    std::atomic<bool> is_synchronization_paused{false};
    bool enable_auto_recovery{true};
    bool enable_performance_tracking{true};
    std::chrono::milliseconds recovery_timeout{1000};
    RecoveryStrategy recovery_strategy{RecoveryStrategy::Automatic};
    
    std::unique_ptr<SyncState> last_good_state;
    PerformanceMetrics performance_metrics;
    
    std::function<void(const ErrorInfo&)> error_handler;
    std::function<void(bool)> recovery_callback;
    
    double sync_threshold = 0.8;
    double stability_threshold = 0.8;
    double coherence_threshold = 0.8;
    size_t history_size = 10;
    
    std::function<void()> custom_recovery_strategy;
    std::function<void(double)> sync_callback;
    std::function<void(const std::exception&)> error_callback;
    
    // Pattern analysis
    struct PatternHistory {
        std::deque<std::vector<double>> recent_patterns;
        std::unordered_map<std::string, size_t> pattern_frequencies;
        std::chrono::system_clock::time_point last_analysis;
    };
    
    PatternHistory pattern_history;
    static constexpr size_t MAX_PATTERN_HISTORY = 1000;
    
    PatternAnalysis perform_pattern_analysis() const;
    
    // Error tracking
    struct ErrorHistory {
        std::queue<ErrorInfo> recent_errors;
        std::unordered_map<std::string, size_t> error_frequencies;
        std::chrono::system_clock::time_point last_prediction;
    };
    
    ErrorHistory error_history;
    static constexpr size_t MAX_ERROR_HISTORY = 100;
    
    ErrorPrediction perform_error_prediction() const;
    OptimizationMetrics calculate_optimization_metrics() const;
    StateAnalysis perform_state_analysis() const;
    bool perform_config_validation(const SyncConfig& config) const;
    
    // Pattern analysis data
    struct PatternAnalysisData {
        std::vector<std::vector<double>> pattern_sequences;
        std::unordered_map<std::string, double> pattern_scores;
        std::chrono::system_clock::time_point last_analysis;
    };
    
    PatternAnalysisData pattern_analysis;
    static constexpr size_t MAX_PATTERN_SEQUENCES = 100;
    
    PatternMetrics calculate_pattern_metrics() const;
    OptimizationStrategy optimization_strategy;
    
    void apply_optimization();
    HealthMetrics calculate_health_metrics() const;
    PatternMatch perform_pattern_matching(const std::vector<double>& pattern) const;
    ErrorAnalysis perform_error_analysis(const ErrorInfo& error) const;
    PatternPrediction perform_pattern_prediction() const;
    
    AdaptiveOptimizationConfig adaptive_config;
    void apply_adaptive_optimization();
    std::vector<PatternCluster> perform_pattern_clustering() const;
    AnomalyDetection perform_anomaly_detection() const;
    PerformanceProfile calculate_performance_profile() const;
    
    MLConfig ml_config;
    
    // Use raw pointers for now to avoid unique_ptr issues with incomplete types
    MLModel* ml_model{nullptr};
    RealTimeOptimizer* real_time_optimizer{nullptr};
    PatternRecognizer* pattern_recognizer{nullptr};
    ErrorPredictor* error_predictor{nullptr};
    PerformanceOptimizer* performance_optimizer{nullptr};
    
    std::thread auto_tuning_thread;
    std::thread adaptive_thresholds_thread;
    std::thread pattern_evolution_thread;
    std::thread root_cause_analysis_thread;
    std::thread resource_optimization_thread;
    
    // Internal utilities
    void resize_histories();
    void handle_error(const std::exception& e);
    void log_error_details(const std::exception& e);
    void attempt_error_recovery();
    void reset_to_last_good_state();
    void collect_errors();
    void update_ml_model(const OptimizationMetrics& metrics);
    void update_real_time_optimizer(const PerformanceMetrics& metrics);
    void update_pattern_recognizer(const std::vector<std::vector<double>>& patterns);
    void update_error_predictor(const std::vector<ErrorInfo>& errors);
    void update_performance_optimizer(const PerformanceMetrics& metrics);
    
    double calculate_pattern_confidence() const;
    std::vector<double> extract_pattern_signature() const;
    std::string classify_pattern() const;
    double calculate_error_probability() const;
    std::string predict_error_type() const;
    std::chrono::system_clock::time_point predict_error_time() const;
    std::vector<double> identify_contributing_factors() const;
    double calculate_cpu_usage() const;
    double calculate_memory_usage() const;
    double calculate_sync_efficiency() const;
    std::vector<double> generate_optimization_suggestions() const;
    double calculate_health_score() const;
    std::vector<std::string> identify_potential_issues() const;
    std::vector<std::string> generate_recommendations() const;
    bool validate_thresholds(const SyncConfig& config) const;
    bool validate_history_size(const SyncConfig& config) const;
    bool validate_timeouts(const SyncConfig& config) const;
    bool validate_performance_settings(const SyncConfig& config) const;
    
    void update_performance_metrics(std::chrono::microseconds duration);
    void initialize_sync_points();
    void initialize_sync_patterns();
    void initialize_sync_metrics();
    void manage_sync_points();
    void manage_sync_patterns();
    void adjust_sync_point(size_t index);
    void adjust_sync_pattern(size_t index);
    double calculate_stability(const std::vector<double>& values);
    double calculate_coherence(const std::vector<double>& values);
    double calculate_sync_adjustment(const std::vector<double>& history);
    void update_sync_point_history();
    void update_sync_pattern_history();
    void update_sync_metrics();
    double calculate_overall_sync();
    double calculate_overall_stability();
    double calculate_overall_coherence();
    void verify_synchronization();
    
    double calculate_stability_score() const;
    double calculate_coherence_score() const;
    double calculate_complexity_score() const;
    std::vector<double> calculate_pattern_weights() const;
    void adjust_parameters(const OptimizationMetrics& metrics);
    double calculate_system_health() const;
    double calculate_component_health() const;
    double calculate_resource_health() const;
    std::vector<std::string> identify_health_indicators() const;
    double calculate_pattern_similarity(const std::vector<double>& pattern) const;
    std::string find_best_match(const std::vector<double>& pattern) const;
    std::vector<double> calculate_match_confidence(const std::vector<double>& pattern) const;
    double calculate_error_severity(const ErrorInfo& error) const;
    std::string identify_root_cause(const ErrorInfo& error) const;
    std::vector<std::string> assess_error_impact(const ErrorInfo& error) const;
    
    std::vector<double> predict_next_values() const;
    double calculate_prediction_confidence() const;
    std::vector<std::string> identify_influencing_factors() const;
    void update_optimization_weights(const OptimizationMetrics& metrics);
    void apply_optimization_changes();
    std::vector<std::vector<double>> collect_patterns() const;
    std::vector<PatternCluster> k_means_clustering(const std::vector<std::vector<double>>& patterns, size_t k) const;
    void evaluate_cluster_quality(std::vector<PatternCluster>& clusters) const;
    bool detect_anomaly() const;
    double calculate_anomaly_score() const;
    std::string classify_anomaly() const;
    std::vector<std::string> identify_anomaly_factors() const;
    std::vector<double> collect_cpu_usage_history() const;
    std::vector<double> collect_memory_usage_history() const;
    std::vector<double> collect_efficiency_history() const;
    std::vector<std::chrono::microseconds> collect_operation_durations() const;
    std::unordered_map<std::string, double> calculate_component_metrics() const;

    void initialize_ml_model();
    void start_auto_tuning();
    void tune_ml_model();
    void initialize_real_time_optimizer();
    void start_adaptive_thresholds();
    void adjust_thresholds();
    void initialize_pattern_recognizer();
    void start_pattern_evolution();
    void evolve_patterns();
    void initialize_error_predictor();
    void start_root_cause_analysis();
    void analyze_root_causes();
    void initialize_performance_optimizer();
    void start_resource_optimization();
    void optimize_resources();
};

}} // namespace chronovyan::sync 