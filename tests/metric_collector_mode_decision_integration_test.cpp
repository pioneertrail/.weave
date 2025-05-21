#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "chronovyan/metric_collector.hpp"
#include "chronovyan/mode_decision_engine.hpp"
#include <chrono>
#include <memory>
#include <stdexcept>

using namespace chronovyan;
using namespace testing;
using namespace std::chrono_literals;

// Mock metric source implementation (reusing from metric_collector_test.cpp)
class MockMetricSourceImpl : public IMetricSource {
public:
    MOCK_CONST_METHOD0(getValue, double());
    MOCK_CONST_METHOD0(isAvailable, bool());
    MOCK_CONST_METHOD0(getLastUpdateTime, std::chrono::system_clock::time_point());
};

class MetricCollectorModeDecisionIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create mock sources
        cpu_source = std::make_unique<MockMetricSourceImpl>();
        memory_source = std::make_unique<MockMetricSourceImpl>();
        gpu_source = std::make_unique<MockMetricSourceImpl>();
        
        // Set up default behaviors
        ON_CALL(*cpu_source, isAvailable()).WillByDefault(Return(true));
        ON_CALL(*memory_source, isAvailable()).WillByDefault(Return(true));
        ON_CALL(*gpu_source, isAvailable()).WillByDefault(Return(true));
        
        // Create collector with mock sources
        collector = std::make_unique<MetricCollector>(
            cpu_source.get(),
            memory_source.get(),
            gpu_source.get()
        );
        
        // Create mode decision engine
        decision_engine = std::make_unique<ModeDecisionEngine>();
    }
    
    // Helper method to set up normal metrics
    void setupNormalMetrics() {
        EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(45.5));
        EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
        EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
        
        EXPECT_CALL(*cpu_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        EXPECT_CALL(*memory_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        EXPECT_CALL(*gpu_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
    }
    
    std::unique_ptr<MockMetricSourceImpl> cpu_source;
    std::unique_ptr<MockMetricSourceImpl> memory_source;
    std::unique_ptr<MockMetricSourceImpl> gpu_source;
    std::unique_ptr<MetricCollector> collector;
    std::unique_ptr<ModeDecisionEngine> decision_engine;
};

// Test handling of stale metrics
TEST_F(MetricCollectorModeDecisionIntegrationTest, HandlesStaleMetricsFromCollector) {
    // Set up stale metrics
    auto old_time = std::chrono::system_clock::now() - 2s;
    auto current_time = std::chrono::system_clock::now();
    
    EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillOnce(Return(old_time));
    EXPECT_CALL(*memory_source, getLastUpdateTime()).WillOnce(Return(current_time));
    EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillOnce(Return(current_time));
    
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(45.5));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics and evaluate
    auto metrics = collector->collect_metrics();
    auto decision = decision_engine->evaluate_metrics(metrics);
    
    // Verify decision is conservative
    EXPECT_EQ(decision.mode, PerformanceMode::Balanced);
    EXPECT_TRUE(decision.is_conservative);
    EXPECT_TRUE(decision.reason.find("stale") != std::string::npos);
}

// Test handling of default values from collector on failure
TEST_F(MetricCollectorModeDecisionIntegrationTest, HandlesDefaultValuesFromCollectorOnFailure) {
    // Simulate CPU sensor failure
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics and evaluate
    auto metrics = collector->collect_metrics();
    auto decision = decision_engine->evaluate_metrics(metrics);
    
    // Verify decision is conservative due to default values
    EXPECT_EQ(decision.mode, PerformanceMode::Lean);
    EXPECT_TRUE(decision.is_conservative);
    EXPECT_TRUE(decision.reason.find("default") != std::string::npos);
}

// Test handling of intermittent metric availability
TEST_F(MetricCollectorModeDecisionIntegrationTest, HandlesIntermittentMetricAvailability) {
    // First collection: normal metrics
    setupNormalMetrics();
    auto metrics1 = collector->collect_metrics();
    auto decision1 = decision_engine->evaluate_metrics(metrics1);
    EXPECT_EQ(decision1.mode, PerformanceMode::Balanced);
    
    // Second collection: CPU sensor failure
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    auto metrics2 = collector->collect_metrics();
    auto decision2 = decision_engine->evaluate_metrics(metrics2);
    EXPECT_EQ(decision2.mode, PerformanceMode::Lean);
    
    // Third collection: normal metrics again
    setupNormalMetrics();
    auto metrics3 = collector->collect_metrics();
    auto decision3 = decision_engine->evaluate_metrics(metrics3);
    
    // Verify hysteresis prevents rapid mode switching
    EXPECT_EQ(decision3.mode, PerformanceMode::Lean);
    EXPECT_TRUE(decision3.reason.find("hysteresis") != std::string::npos);
}

// Test handling of complete metric collector failure
TEST_F(MetricCollectorModeDecisionIntegrationTest, HandlesCompleteMetricCollectorFailure) {
    // Simulate complete failure of all sensors
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*gpu_source, isAvailable()).WillOnce(Return(false));
    
    // Collect metrics and evaluate
    auto metrics = collector->collect_metrics();
    auto decision = decision_engine->evaluate_metrics(metrics);
    
    // Verify system-wide fallback to safe mode
    EXPECT_EQ(decision.mode, PerformanceMode::Lean);
    EXPECT_TRUE(decision.is_conservative);
    EXPECT_TRUE(decision.reason.find("critical") != std::string::npos);
    EXPECT_TRUE(decision.requires_fallback);
}

// Test handling of out-of-range metrics
TEST_F(MetricCollectorModeDecisionIntegrationTest, HandlesOutOfRangeMetrics) {
    // Set up out-of-range metrics
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(150.0));  // > 100%
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(-10.0));  // < 0%
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics and evaluate
    auto metrics = collector->collect_metrics();
    auto decision = decision_engine->evaluate_metrics(metrics);
    
    // Verify decision is conservative
    EXPECT_EQ(decision.mode, PerformanceMode::Lean);
    EXPECT_TRUE(decision.is_conservative);
    EXPECT_TRUE(decision.reason.find("range") != std::string::npos);
}

// Test handling of NaN metrics
TEST_F(MetricCollectorModeDecisionIntegrationTest, HandlesNaNMetrics) {
    // Set up NaN metrics
    EXPECT_CALL(*cpu_source, getValue())
        .WillOnce(Return(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics and evaluate
    auto metrics = collector->collect_metrics();
    auto decision = decision_engine->evaluate_metrics(metrics);
    
    // Verify decision is conservative
    EXPECT_EQ(decision.mode, PerformanceMode::Lean);
    EXPECT_TRUE(decision.is_conservative);
    EXPECT_TRUE(decision.reason.find("invalid") != std::string::npos);
}

// Test handling of rapid metric changes
TEST_F(MetricCollectorModeDecisionIntegrationTest, HandlesRapidMetricChanges) {
    // First collection: normal metrics
    setupNormalMetrics();
    auto metrics1 = collector->collect_metrics();
    auto decision1 = decision_engine->evaluate_metrics(metrics1);
    EXPECT_EQ(decision1.mode, PerformanceMode::Balanced);
    
    // Second collection: sudden high load
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(95.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(90.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(85.0));
    
    EXPECT_CALL(*cpu_source, getLastUpdateTime())
        .WillOnce(Return(std::chrono::system_clock::now()));
    EXPECT_CALL(*memory_source, getLastUpdateTime())
        .WillOnce(Return(std::chrono::system_clock::now()));
    EXPECT_CALL(*gpu_source, getLastUpdateTime())
        .WillOnce(Return(std::chrono::system_clock::now()));
    
    auto metrics2 = collector->collect_metrics();
    auto decision2 = decision_engine->evaluate_metrics(metrics2);
    
    // Verify hysteresis prevents rapid mode switching
    EXPECT_EQ(decision2.mode, PerformanceMode::Balanced);
    EXPECT_TRUE(decision2.reason.find("hysteresis") != std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 