#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "chronovyan/metric_collector.hpp"
#include <chrono>
#include <memory>
#include <stdexcept>

using namespace chronovyan;
using namespace testing;
using namespace std::chrono_literals;

// Mock metric source interface
class MockMetricSource {
public:
    virtual ~MockMetricSource() = default;
    virtual double getValue() const = 0;
    virtual bool isAvailable() const = 0;
    virtual std::chrono::system_clock::time_point getLastUpdateTime() const = 0;
};

// Mock implementation
class MockMetricSourceImpl : public IMetricSource {
public:
    MOCK_CONST_METHOD0(getValue, double());
    MOCK_CONST_METHOD0(isAvailable, bool());
    MOCK_CONST_METHOD0(getLastUpdateTime, std::chrono::system_clock::time_point());
};

class MetricCollectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create mock sources for different metrics
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
    }
    
    std::unique_ptr<MockMetricSourceImpl> cpu_source;
    std::unique_ptr<MockMetricSourceImpl> memory_source;
    std::unique_ptr<MockMetricSourceImpl> gpu_source;
    std::unique_ptr<MetricCollector> collector;
};

// Test successful metric collection
TEST_F(MetricCollectorTest, CollectsMetricsSuccessfully) {
    // Set up expectations
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(45.5));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    auto current_time = std::chrono::system_clock::now();
    EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    EXPECT_CALL(*memory_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    
    // Collect metrics
    auto metrics = collector->collect_metrics();
    
    // Verify results
    EXPECT_DOUBLE_EQ(metrics.cpu_usage, 45.5);
    EXPECT_DOUBLE_EQ(metrics.memory_usage, 60.0);
    EXPECT_DOUBLE_EQ(metrics.gpu_usage, 75.0);
    EXPECT_FALSE(metrics.is_stale);
}

// Test handling of unavailable metric source
TEST_F(MetricCollectorTest, HandlesUnavailableMetricSource) {
    // Set up expectations
    EXPECT_CALL(*cpu_source, isAvailable()).WillRepeatedly(Return(false));
    EXPECT_CALL(*memory_source, isAvailable()).WillRepeatedly(Return(true));
    EXPECT_CALL(*gpu_source, isAvailable()).WillRepeatedly(Return(true));
    
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    auto current_time = std::chrono::system_clock::now();
    EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    EXPECT_CALL(*memory_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    
    // Collect metrics
    auto metrics = collector->collect_metrics();
    
    // Verify results
    EXPECT_DOUBLE_EQ(metrics.cpu_usage, MetricCollector::kDefaultCpuUsage);
    EXPECT_DOUBLE_EQ(metrics.memory_usage, 60.0);
    EXPECT_DOUBLE_EQ(metrics.gpu_usage, 75.0);
    EXPECT_TRUE(metrics.is_stale);
}

// Test handling of metric source throwing exception
TEST_F(MetricCollectorTest, HandlesMetricSourceException) {
    // Set up expectations
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Throw(std::runtime_error("Sensor error")));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics
    auto metrics = collector->collect_metrics();
    
    // Verify results
    EXPECT_DOUBLE_EQ(metrics.cpu_usage, MetricCollector::kDefaultCpuUsage);
    EXPECT_DOUBLE_EQ(metrics.memory_usage, 60.0);
    EXPECT_DOUBLE_EQ(metrics.gpu_usage, 75.0);
    EXPECT_TRUE(metrics.is_stale);
}

// Test handling of stale metrics
TEST_F(MetricCollectorTest, DetectsStaleMetrics) {
    auto old_time = std::chrono::system_clock::now() - 2s;
    auto current_time = std::chrono::system_clock::now();
    
    // Set up expectations
    EXPECT_CALL(*cpu_source, isAvailable()).WillRepeatedly(Return(true));
    EXPECT_CALL(*memory_source, isAvailable()).WillRepeatedly(Return(true));
    EXPECT_CALL(*gpu_source, isAvailable()).WillRepeatedly(Return(true));
    
    EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillRepeatedly(Return(old_time));
    EXPECT_CALL(*memory_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(45.5));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics
    auto metrics = collector->collect_metrics();
    
    // Verify results
    EXPECT_TRUE(metrics.is_stale);
    EXPECT_DOUBLE_EQ(metrics.cpu_usage, 45.5);
    EXPECT_DOUBLE_EQ(metrics.memory_usage, 60.0);
    EXPECT_DOUBLE_EQ(metrics.gpu_usage, 75.0);
}

// Test handling of out-of-range metrics
TEST_F(MetricCollectorTest, HandlesOutOfRangeMetrics) {
    // Set up expectations
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(150.0));  // > 100%
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(-10.0));  // < 0%
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics
    auto metrics = collector->collect_metrics();
    
    // Verify results
    EXPECT_DOUBLE_EQ(metrics.cpu_usage, 100.0);  // Clamped to max
    EXPECT_DOUBLE_EQ(metrics.memory_usage, 0.0);  // Clamped to min
    EXPECT_DOUBLE_EQ(metrics.gpu_usage, 75.0);
    EXPECT_TRUE(metrics.is_stale);
}

// Test handling of NaN metrics
TEST_F(MetricCollectorTest, HandlesNaNMetrics) {
    // Set up expectations
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Collect metrics
    auto metrics = collector->collect_metrics();
    
    // Verify results
    EXPECT_DOUBLE_EQ(metrics.cpu_usage, MetricCollector::kDefaultCpuUsage);
    EXPECT_DOUBLE_EQ(metrics.memory_usage, 60.0);
    EXPECT_DOUBLE_EQ(metrics.gpu_usage, 75.0);
    EXPECT_TRUE(metrics.is_stale);
}

// Test handling of multiple failures
TEST_F(MetricCollectorTest, HandlesMultipleFailures) {
    // Set up expectations
    EXPECT_CALL(*cpu_source, isAvailable()).WillRepeatedly(Return(false));
    EXPECT_CALL(*memory_source, isAvailable()).WillRepeatedly(Return(true));
    EXPECT_CALL(*gpu_source, isAvailable()).WillRepeatedly(Return(true));
    
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Throw(std::runtime_error("Memory sensor error")));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(std::numeric_limits<double>::quiet_NaN()));
    
    auto current_time = std::chrono::system_clock::now();
    EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    EXPECT_CALL(*memory_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
    
    // Collect metrics
    auto metrics = collector->collect_metrics();
    
    // Verify results
    EXPECT_DOUBLE_EQ(metrics.cpu_usage, MetricCollector::kDefaultCpuUsage);
    EXPECT_DOUBLE_EQ(metrics.memory_usage, MetricCollector::kDefaultMemoryUsage);
    EXPECT_DOUBLE_EQ(metrics.gpu_usage, MetricCollector::kDefaultGpuUsage);
    EXPECT_TRUE(metrics.is_stale);
}

// Test recovery after temporary failure
TEST_F(MetricCollectorTest, RecoversAfterTemporaryFailure) {
    auto current_time = std::chrono::system_clock::now();
    
    // First collection with failure
    {
        EXPECT_CALL(*cpu_source, isAvailable()).WillRepeatedly(Return(false));
        EXPECT_CALL(*memory_source, isAvailable()).WillRepeatedly(Return(true));
        EXPECT_CALL(*gpu_source, isAvailable()).WillRepeatedly(Return(true));
        
        EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
        EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
        
        EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
        EXPECT_CALL(*memory_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
        EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
        
        auto metrics1 = collector->collect_metrics();
        EXPECT_DOUBLE_EQ(metrics1.cpu_usage, MetricCollector::kDefaultCpuUsage);
        EXPECT_TRUE(metrics1.is_stale);
    }
    
    // Second collection with recovery
    {
        EXPECT_CALL(*cpu_source, isAvailable()).WillRepeatedly(Return(true));
        EXPECT_CALL(*memory_source, isAvailable()).WillRepeatedly(Return(true));
        EXPECT_CALL(*gpu_source, isAvailable()).WillRepeatedly(Return(true));
        
        EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(45.5));
        EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
        EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
        
        EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
        EXPECT_CALL(*memory_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
        EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillRepeatedly(Return(current_time));
        
        auto metrics2 = collector->collect_metrics();
        EXPECT_DOUBLE_EQ(metrics2.cpu_usage, 45.5);
        EXPECT_FALSE(metrics2.is_stale);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 