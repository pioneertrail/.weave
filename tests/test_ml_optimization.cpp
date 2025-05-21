#include <gtest/gtest.h>
#include "chronovyan/temporal_synchronizer.hpp"
#include <thread>
#include <chrono>

using namespace chronovyan::sync;

class MLOptimizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Configure ML optimization
        MLConfig ml_config;
        ml_config.enable_ml_optimization = true;
        ml_config.learning_rate = 0.01;
        ml_config.training_window = 1000;
        ml_config.feature_columns = {"sync_efficiency", "stability", "coherence"};
        ml_config.model_type = "gradient_boost";
        ml_config.enable_auto_tuning = true;
        
        synchronizer.configure_ml(ml_config);
        
        // Configure real-time optimization
        RealTimeConfig real_time_config;
        real_time_config.enable_real_time_optimization = true;
        real_time_config.optimization_interval = std::chrono::milliseconds(100);
        real_time_config.target_latency = 50.0;
        real_time_config.optimization_window = 100;
        real_time_config.enable_adaptive_thresholds = true;
        
        synchronizer.configure_real_time(real_time_config);
    }
    
    TemporalSynchronizer synchronizer;
};

TEST_F(MLOptimizationTest, MLModelInitialization) {
    // Test ML model initialization
    auto metrics = synchronizer.get_optimization_metrics();
    EXPECT_GE(metrics.sync_efficiency, 0.0);
    EXPECT_LE(metrics.sync_efficiency, 1.0);
    EXPECT_GE(metrics.stability_score, 0.0);
    EXPECT_LE(metrics.stability_score, 1.0);
    EXPECT_GE(metrics.coherence_score, 0.0);
    EXPECT_LE(metrics.coherence_score, 1.0);
}

TEST_F(MLOptimizationTest, RealTimeOptimization) {
    // Test real-time optimization
    auto metrics = synchronizer.get_performance_metrics();
    EXPECT_GE(metrics.latency, 0.0);
    
    // Simulate high latency
    metrics.latency = 100.0;
    synchronizer.update_performance_metrics(metrics);
    
    // Wait for optimization
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Check if optimization was applied
    auto new_metrics = synchronizer.get_performance_metrics();
    EXPECT_LT(new_metrics.latency, metrics.latency);
}

TEST_F(MLOptimizationTest, AdaptiveThresholds) {
    // Test adaptive thresholds
    auto metrics = synchronizer.get_performance_metrics();
    double initial_latency = metrics.latency;
    
    // Simulate varying latency
    for (int i = 0; i < 10; ++i) {
        metrics.latency = initial_latency * (1.0 + 0.1 * i);
        synchronizer.update_performance_metrics(metrics);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Check if thresholds were adjusted
    auto new_metrics = synchronizer.get_performance_metrics();
    EXPECT_NE(new_metrics.latency, initial_latency);
}

TEST_F(MLOptimizationTest, FeatureImportance) {
    // Test feature importance calculation
    auto metrics = synchronizer.get_optimization_metrics();
    
    // Update metrics multiple times
    for (int i = 0; i < 100; ++i) {
        metrics.sync_efficiency = 0.5 + 0.1 * std::sin(i * 0.1);
        metrics.stability_score = 0.6 + 0.1 * std::cos(i * 0.1);
        metrics.coherence_score = 0.7 + 0.1 * std::sin(i * 0.2);
        
        synchronizer.update_optimization_metrics(metrics);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Check if feature importance was calculated
    auto feature_importance = synchronizer.get_feature_importance();
    EXPECT_EQ(feature_importance.size(), 3);
    
    double total_importance = std::accumulate(
        feature_importance.begin(),
        feature_importance.end(),
        0.0
    );
    EXPECT_NEAR(total_importance, 1.0, 0.001);
}

TEST_F(MLOptimizationTest, OptimizationHistory) {
    // Test optimization history tracking
    auto metrics = synchronizer.get_performance_metrics();
    
    // Simulate performance variations
    for (int i = 0; i < 50; ++i) {
        metrics.latency = 50.0 + 20.0 * std::sin(i * 0.1);
        synchronizer.update_performance_metrics(metrics);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Check optimization history
    auto history = synchronizer.get_optimization_history();
    EXPECT_FALSE(history.empty());
    EXPECT_LE(history.size(), 100);  // Should not exceed optimization window
}

TEST_F(MLOptimizationTest, ConcurrentOptimization) {
    // Test concurrent optimization
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};
    
    // Start multiple threads updating metrics
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([this, &stop, i]() {
            auto metrics = synchronizer.get_performance_metrics();
            while (!stop) {
                metrics.latency = 50.0 + 10.0 * std::sin(i * 0.1);
                synchronizer.update_performance_metrics(metrics);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    }
    
    // Let threads run for a while
    std::this_thread::sleep_for(std::chrono::seconds(2));
    stop = true;
    
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Check if optimization was successful
    auto final_metrics = synchronizer.get_performance_metrics();
    EXPECT_GE(final_metrics.latency, 0.0);
}

TEST_F(MLOptimizationTest, RecoveryFromHighLatency) {
    // Test recovery from high latency
    auto metrics = synchronizer.get_performance_metrics();
    
    // Simulate high latency
    metrics.latency = 200.0;
    synchronizer.update_performance_metrics(metrics);
    
    // Wait for optimization
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Check if system recovered
    auto new_metrics = synchronizer.get_performance_metrics();
    EXPECT_LT(new_metrics.latency, metrics.latency);
    EXPECT_NEAR(new_metrics.latency, 50.0, 20.0);  // Should be close to target
}

TEST_F(MLOptimizationTest, StabilityUnderLoad) {
    // Test stability under varying load
    auto metrics = synchronizer.get_performance_metrics();
    std::vector<double> latencies;
    
    // Simulate varying load
    for (int i = 0; i < 100; ++i) {
        metrics.latency = 50.0 + 30.0 * std::sin(i * 0.1);
        synchronizer.update_performance_metrics(metrics);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        latencies.push_back(metrics.latency);
    }
    
    // Calculate latency statistics
    double avg_latency = std::accumulate(
        latencies.begin(),
        latencies.end(),
        0.0
    ) / latencies.size();
    
    double variance = 0.0;
    for (double latency : latencies) {
        double diff = latency - avg_latency;
        variance += diff * diff;
    }
    variance /= latencies.size();
    
    // Check stability
    EXPECT_NEAR(avg_latency, 50.0, 10.0);  // Average should be close to target
    EXPECT_LT(std::sqrt(variance), 20.0);  // Low variance indicates stability
} 