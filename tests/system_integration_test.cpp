#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "chronovyan/metric_collector.hpp"
#include "chronovyan/mode_decision_engine.hpp"
#include "chronovyan/state_controller.hpp"
#include "chronovyan/notification_service.hpp"
#include "chronovyan/common_types.hpp"
#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace chronovyan;
using namespace testing;
using namespace std::chrono_literals;

// Mock notification service
class MockNotificationService : public INotificationService {
public:
    MOCK_METHOD(void, notifyModeChange, (PerformanceMode new_mode, const std::string& reason), (override));
    MOCK_METHOD(void, notifyError, (const std::string& error_message), (override));
};

// Mock metric source implementation
class MockMetricSourceImpl : public IMetricSource {
public:
    MOCK_CONST_METHOD0(getValue, double());
    MOCK_CONST_METHOD0(isAvailable, bool());
    MOCK_CONST_METHOD0(getLastUpdateTime, std::chrono::system_clock::time_point());
};

class SystemIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create mock objects
        cpu_source = std::make_unique<MockMetricSourceImpl>();
        memory_source = std::make_unique<MockMetricSourceImpl>();
        gpu_source = std::make_unique<MockMetricSourceImpl>();
        notification_service = std::make_unique<MockNotificationService>();
        
        // Set default behavior for mocks
        ON_CALL(*cpu_source, isAvailable()).WillByDefault(Return(true));
        ON_CALL(*memory_source, isAvailable()).WillByDefault(Return(true));
        ON_CALL(*gpu_source, isAvailable()).WillByDefault(Return(true));
        
        // Create the system
        collector = std::make_unique<MetricCollector>();
        collector->addSource("cpu", cpu_source.get());
        collector->addSource("memory", memory_source.get());
        collector->addSource("gpu", gpu_source.get());
        
        decision_engine = std::make_unique<ModeDecisionEngine>();
        state_controller = std::make_unique<StateController>(notification_service.get());
        
        // Disable test forcing by default
        decision_engine->setForceModeForTesting(PerformanceMode::Balanced, false);
        StateController::setForceCooldownForTesting(false);
    }
    
    void TearDown() override {
        // Clean up any test-specific state
        decision_engine->setForceModeForTesting(PerformanceMode::Balanced, false);
        StateController::setForceCooldownForTesting(false);
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
    
    // Helper method to process metrics through the system
    void processMetrics() {
        auto metrics = collector->collect_metrics();
        auto decision = decision_engine->evaluate_metrics(metrics);
        state_controller->updateMode(decision);
    }
    
    std::unique_ptr<MockMetricSourceImpl> cpu_source;
    std::unique_ptr<MockMetricSourceImpl> memory_source;
    std::unique_ptr<MockMetricSourceImpl> gpu_source;
    std::unique_ptr<MetricCollector> collector;
    std::unique_ptr<ModeDecisionEngine> decision_engine;
    std::unique_ptr<MockNotificationService> notification_service;
    std::unique_ptr<StateController> state_controller;
};

// Test normal mode switching
TEST_F(SystemIntegrationTest, SwitchesModeCorrectly_OnDecisionEngineOutput) {
    // Set up metrics that should trigger a switch to Balanced mode
    setupNormalMetrics();
    
    // Expect notification
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("normal")
    )).Times(1);
    
    // Process metrics
    processMetrics();
    
    // Verify state
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test cooldown enforcement
TEST_F(SystemIntegrationTest, EnforcesCooldown_AfterModeSwitch) {
    // First switch to Balanced
    setupNormalMetrics();
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("normal")
    )).Times(1);
    processMetrics();
    
    // Immediately try to switch to HighFidelity
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));  // Low CPU
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));  // Low memory
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));  // Low GPU
    
    EXPECT_CALL(*cpu_source, getLastUpdateTime())
        .WillOnce(Return(std::chrono::system_clock::now()));
    EXPECT_CALL(*memory_source, getLastUpdateTime())
        .WillOnce(Return(std::chrono::system_clock::now()));
    EXPECT_CALL(*gpu_source, getLastUpdateTime())
        .WillOnce(Return(std::chrono::system_clock::now()));
    
    // Process metrics again
    processMetrics();
    
    // Verify state hasn't changed due to cooldown
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test fallback to Lean mode on critical failure
TEST_F(SystemIntegrationTest, HandlesFallbackToLean_WhenDecisionEngineIndicatesCriticalFailure) {
    // Simulate complete sensor failure
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*gpu_source, isAvailable()).WillOnce(Return(false));
    
    // Expect error notification
    EXPECT_CALL(*notification_service, notifyError(
        testing::HasSubstr("critical")
    )).Times(1);
    
    // Expect mode change notification
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Lean,
        testing::HasSubstr("critical")
    )).Times(1);
    
    // Process metrics
    processMetrics();
    
    // Verify state
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
}

// Test recovery from temporary failure
TEST_F(SystemIntegrationTest, RecoversFromTemporaryFailure_AndRestoresAppropriateMode) {
    // First, simulate failure and switch to Lean
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Lean,
        testing::HasSubstr("default")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Then provide healthy metrics that would justify Balanced mode
    setupNormalMetrics();
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("normal")
    )).Times(1);
    
    processMetrics();
    
    // Verify state has recovered
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of rapid metric fluctuations
TEST_F(SystemIntegrationTest, HandlesRapidMetricFluctuations) {
    // First switch to Balanced
    setupNormalMetrics();
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("normal")
    )).Times(1);
    processMetrics();
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Simulate rapid fluctuations between high and low load
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            // High load
            EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(95.0));
            EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(90.0));
            EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(85.0));
        } else {
            // Low load
            EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
            EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
            EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
        }
        
        EXPECT_CALL(*cpu_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        EXPECT_CALL(*memory_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        EXPECT_CALL(*gpu_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        
        processMetrics();
    }
    
    // Verify state remains stable
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of stale metrics with mode switching
TEST_F(SystemIntegrationTest, HandlesStaleMetrics_WithModeSwitching) {
    // First switch to Balanced
    setupNormalMetrics();
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("normal")
    )).Times(1);
    processMetrics();
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Provide stale metrics
    auto old_time = std::chrono::system_clock::now() - 2s;
    auto current_time = std::chrono::system_clock::now();
    
    EXPECT_CALL(*cpu_source, getLastUpdateTime()).WillOnce(Return(old_time));
    EXPECT_CALL(*memory_source, getLastUpdateTime()).WillOnce(Return(current_time));
    EXPECT_CALL(*gpu_source, getLastUpdateTime()).WillOnce(Return(current_time));
    
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(45.5));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    // Process metrics
    processMetrics();
    
    // Verify state remains unchanged due to stale metrics
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of partial sensor failures
TEST_F(SystemIntegrationTest, HandlesPartialSensorFailures) {
    // Simulate CPU sensor failure while others are healthy
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    EXPECT_CALL(*notification_service, notifyError(
        testing::HasSubstr("partial")
    )).Times(1);
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("partial")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of metric source exceptions
TEST_F(SystemIntegrationTest, HandlesMetricSourceExceptions) {
    // Simulate CPU sensor throwing an exception
    EXPECT_CALL(*cpu_source, getValue())
        .WillOnce(Throw(std::runtime_error("Sensor error")));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    EXPECT_CALL(*notification_service, notifyError(
        testing::HasSubstr("exception")
    )).Times(1);
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("exception")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of metric source timeouts
TEST_F(SystemIntegrationTest, HandlesMetricSourceTimeouts) {
    // Simulate CPU sensor taking too long to respond
    EXPECT_CALL(*cpu_source, getValue())
        .WillOnce(Invoke([]() {
            std::this_thread::sleep_for(2s);
            return 45.5;
        }));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    EXPECT_CALL(*notification_service, notifyError(
        testing::HasSubstr("timeout")
    )).Times(1);
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("timeout")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of metric source recovery after multiple failures
TEST_F(SystemIntegrationTest, HandlesMetricSourceRecoveryAfterMultipleFailures) {
    // First failure
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("default")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
    
    // Second failure
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Recovery
    setupNormalMetrics();
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("recovered")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of metric source degradation
TEST_F(SystemIntegrationTest, HandlesMetricSourceDegradation) {
    // Simulate gradual degradation of CPU sensor
    for (int i = 0; i < 5; ++i) {
        if (i < 3) {
            // Normal operation
            setupNormalMetrics();
        } else {
            // Degraded operation
            EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
            EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
            EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
        }
        
        EXPECT_CALL(*notification_service, notifyModeChange(
            PerformanceMode::Balanced,
            testing::HasSubstr(i < 3 ? "normal" : "degraded")
        )).Times(1);
        
        processMetrics();
        EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
        
        if (i < 4) {
            std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
        }
    }
}

// Test handling of metric source calibration
TEST_F(SystemIntegrationTest, HandlesMetricSourceCalibration) {
    // Simulate CPU sensor requiring calibration
    EXPECT_CALL(*cpu_source, getValue())
        .WillOnce(Return(0.0))  // Initial reading
        .WillOnce(Return(0.0))  // Calibration in progress
        .WillOnce(Return(45.5)); // Calibrated reading
    
    EXPECT_CALL(*memory_source, getValue())
        .Times(3)
        .WillRepeatedly(Return(60.0));
    
    EXPECT_CALL(*gpu_source, getValue())
        .Times(3)
        .WillRepeatedly(Return(75.0));
    
    // First reading
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("calibrating")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Second reading
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Third reading (calibrated)
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Balanced,
        testing::HasSubstr("calibrated")
    )).Times(1);
    
    processMetrics();
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
}

// Test handling of metric source drift
TEST_F(SystemIntegrationTest, HandlesMetricSourceDrift) {
    // Simulate CPU sensor drift
    double drift_values[] = {45.5, 47.0, 48.5, 50.0, 51.5};
    
    for (double value : drift_values) {
        EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(value));
        EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
        EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
        
        EXPECT_CALL(*cpu_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        EXPECT_CALL(*memory_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        EXPECT_CALL(*gpu_source, getLastUpdateTime())
            .WillOnce(Return(std::chrono::system_clock::now()));
        
        processMetrics();
        EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Balanced);
        
        std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    }
}

// Enhanced assertions for basic mode switching
TEST_F(SystemIntegrationTest, StateController_CorrectlyAppliesModeSwitch_FromDecisionEngine) {
    // Set up metrics for HighFidelity mode
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));  // Low CPU
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));  // Low memory
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));  // Low GPU
    
    // Verify notification details
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::HighFidelity,
        std::string("High performance mode activated due to high CPU usage")
    )).Times(1);
    
    // Verify no error notifications
    EXPECT_CALL(*notification_service, notifyError(_)).Times(0);
    
    processMetrics();
    
    // Verify current state
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::HighFidelity);
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInCooldown());
    EXPECT_FALSE(state_controller->isInErrorState());
    
    // Verify mode history
    auto mode_history = state_controller->getModeHistory();
    ASSERT_FALSE(mode_history.empty());
    const auto& last_entry = mode_history.back();
    EXPECT_EQ(last_entry.mode, PerformanceMode::HighFidelity);
    EXPECT_TRUE(last_entry.reason.find("High performance mode activated due to high CPU usage") != std::string::npos);
    EXPECT_FALSE(last_entry.is_fallback);
    EXPECT_FALSE(last_entry.is_error);
    EXPECT_GE(last_entry.timestamp, std::chrono::system_clock::now() - 1s);
    
    // Verify transition history
    auto transitions = state_controller->getModeTransitions();
    ASSERT_EQ(transitions.size(), 1);
    EXPECT_EQ(transitions[0].from, PerformanceMode::Balanced);  // Initial mode
    EXPECT_EQ(transitions[0].to, PerformanceMode::HighFidelity);
    EXPECT_GE(transitions[0].timestamp, std::chrono::system_clock::now() - 1s);
}

// Enhanced assertions for cooldown enforcement
TEST_F(SystemIntegrationTest, StateController_EnforcesCooldown_BetweenSwitchesInitiatedByDecisionEngine) {
    // First switch to HighFidelity
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::HighFidelity,
        std::string("High performance mode activated due to high CPU usage")
    )).Times(1);
    
    processMetrics();
    
    // Verify initial state
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::HighFidelity);
    EXPECT_TRUE(state_controller->isInCooldown());
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    
    // Record initial state for comparison
    auto initial_history = state_controller->getModeHistory();
    auto initial_transitions = state_controller->getModeTransitions();
    auto initial_cooldown_end = state_controller->getCooldownEndTime();
    
    // Immediately try to switch to Lean
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(95.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(90.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(85.0));
    
    // Verify no notifications during cooldown
    EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
    EXPECT_CALL(*notification_service, notifyError(_)).Times(0);
    
    processMetrics();
    
    // Verify state remains unchanged
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::HighFidelity);
    EXPECT_TRUE(state_controller->isInCooldown());
    EXPECT_EQ(state_controller->getModeHistory().size(), initial_history.size());
    EXPECT_EQ(state_controller->getModeTransitions().size(), initial_transitions.size());
    EXPECT_EQ(state_controller->getCooldownEndTime(), initial_cooldown_end);
    
    // Wait for cooldown to expire
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Verify cooldown has expired
    EXPECT_FALSE(state_controller->isInCooldown());
    EXPECT_GT(std::chrono::system_clock::now(), state_controller->getCooldownEndTime());
    
    // Now try the switch again
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Lean,
        std::string("High performance mode activated due to high CPU usage")
    )).Times(1);
    
    processMetrics();
    
    // Verify final state
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
    EXPECT_TRUE(state_controller->isInCooldown());
    EXPECT_GT(state_controller->getModeHistory().size(), initial_history.size());
    EXPECT_GT(state_controller->getModeTransitions().size(), initial_transitions.size());
}

// Enhanced assertions for handling invalid target mode
TEST_F(SystemIntegrationTest, StateController_RejectsInvalidTargetMode_FromDecisionEngine) {
    // Set up metrics that would trigger an invalid mode
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    // Verify error notification details
    EXPECT_CALL(*notification_service, notifyError(
        std::string("Invalid mode transition detected")
    )).Times(1);
    
    // Verify no mode change notification
    EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
    
    // Create a decision with an invalid mode
    auto metrics = collector->collect_metrics();
    auto decision = decision_engine->evaluate_metrics(metrics);
    decision.mode = static_cast<PerformanceMode>(999);  // Invalid mode
    
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // Attempt mode switch
    state_controller->updateMode(decision);
    
    // Verify state remains unchanged
    EXPECT_EQ(state_controller->getCurrentMode(), initial_mode);
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    EXPECT_FALSE(state_controller->isInCooldown());
    
    // Verify history and transitions
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), initial_history_size + 1);  // Error should be logged
    EXPECT_TRUE(mode_history.back().reason.find("Invalid mode transition detected") != std::string::npos);
    EXPECT_TRUE(mode_history.back().is_error);
    EXPECT_FALSE(mode_history.back().is_fallback);
    
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), initial_transitions_size);  // No transition should be recorded
    
    // Verify error state details
    EXPECT_TRUE(state_controller->hasError());
    auto error_details = state_controller->getLastError();
    EXPECT_TRUE(error_details.find("Invalid mode transition detected") != std::string::npos);
}

// Enhanced assertions for preventing mode oscillation
TEST_F(SystemIntegrationTest, StateController_PreventsModeOscillation) {
    std::vector<PerformanceMode> modes = {
        PerformanceMode::HighFidelity,
        PerformanceMode::Balanced,
        PerformanceMode::Lean
    };
    
    // Track mode changes and timestamps
    std::vector<std::chrono::system_clock::time_point> change_timestamps;
    
    // Enable direct mode setting for testing
    StateController::setDirectModeSetForTesting(true);
    
    for (size_t i = 0; i < modes.size(); ++i) {
        if (i > 0) {
            std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
        }
        
        // Force the specific mode and cooldown for testing
        decision_engine->setForceModeForTesting(modes[i], true);
        StateController::setForceCooldownForTesting(true);
        
        // Set up metrics for current mode
        double cpu = (i == 0) ? 20.0 : (i == 1) ? 45.0 : 95.0;
        double memory = (i == 0) ? 30.0 : (i == 1) ? 60.0 : 90.0;
        double gpu = (i == 0) ? 40.0 : (i == 1) ? 75.0 : 85.0;
        
        EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(cpu));
        EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(memory));
        EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(gpu));
        
        EXPECT_CALL(*notification_service, notifyModeChange(
            modes[i],
            std::string(i == 0 ? "High performance mode activated due to high CPU usage" : (i == 1 ? "normal" : "High performance mode activated due to high CPU usage"))
        )).Times(1);
        
        processMetrics();
        
        // Record state after each change
        EXPECT_EQ(state_controller->getCurrentMode(), modes[i]);
        EXPECT_TRUE(state_controller->isInCooldown());
        change_timestamps.push_back(std::chrono::system_clock::now());
        
        // Verify history entry
        auto mode_history = state_controller->getModeHistory();
        ASSERT_FALSE(mode_history.empty());
        const auto& last_entry = mode_history.back();
        EXPECT_EQ(last_entry.mode, modes[i]);
        EXPECT_FALSE(last_entry.is_fallback);
        EXPECT_FALSE(last_entry.is_error);
        EXPECT_GE(last_entry.timestamp, change_timestamps.back() - 1s);
    }
    
    // Try to oscillate back to HighFidelity
    decision_engine->setForceModeForTesting(PerformanceMode::HighFidelity, true);
    
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    // Verify no notifications during oscillation prevention
    EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
    EXPECT_CALL(*notification_service, notifyError(_)).Times(0);
    
    // Disable direct mode setting for the oscillation test
    StateController::setDirectModeSetForTesting(false);
    processMetrics();
    
    // Verify state remains stable
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
    EXPECT_TRUE(state_controller->isInCooldown());
    
    // Verify oscillation prevention
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), modes.size());  // Only the initial transitions
    
    // Verify timing between changes
    for (size_t i = 1; i < change_timestamps.size(); ++i) {
        auto time_between_changes = change_timestamps[i] - change_timestamps[i-1];
        EXPECT_GE(time_between_changes, StateController::kModeSwitchCooldown);
    }
    
    // Verify final state details
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    EXPECT_TRUE(state_controller->isInCooldown());
    
    // Verify transition history
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), modes.size() - 1);  // Number of transitions between modes
    
    // Cleanup
    StateController::setDirectModeSetForTesting(false);
}

// Enhanced assertions for handling conflicting rapid decisions
TEST_F(SystemIntegrationTest, StateController_HandlesConflictingRapidDecisions_FromDecisionEngine) {
    std::vector<PerformanceMode> attempted_modes;
    std::vector<std::string> attempted_reasons;
    std::vector<std::chrono::system_clock::time_point> decision_timestamps;
    
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // Enable direct mode setting for testing
    StateController::setDirectModeSetForTesting(true);
    
    // Force cooldown state for testing
    StateController::setForceCooldownForTesting(true);
    
    // Simulate rapid conflicting decisions
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            // Try to switch to HighFidelity
            decision_engine->setForceModeForTesting(PerformanceMode::HighFidelity, true);
            
            EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
            EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
            EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
            attempted_modes.push_back(PerformanceMode::HighFidelity);
            attempted_reasons.push_back("High performance mode activated due to high CPU usage");
        } else {
            // Try to switch to Lean
            decision_engine->setForceModeForTesting(PerformanceMode::Lean, true);
            
            EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(95.0));
            EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(90.0));
            EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(85.0));
            attempted_modes.push_back(PerformanceMode::Lean);
            attempted_reasons.push_back("high_load");
        }
        
        // Only expect notification for first change
        if (i == 0) {
            EXPECT_CALL(*notification_service, notifyModeChange(
                attempted_modes[0],
                testing::HasSubstr(attempted_reasons[0])
            )).Times(1);
        } else {
            EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
        }
        
        // For the first change use direct setting, then disable it to test cooldown
        if (i == 0) {
            processMetrics();
        } else {
            // Disable direct mode setting after first change
            StateController::setDirectModeSetForTesting(false);
            processMetrics();
        }
        
        decision_timestamps.push_back(std::chrono::system_clock::now());
        
        // Verify state after each attempt
        if (i == 0) {
            EXPECT_EQ(state_controller->getCurrentMode(), attempted_modes[0]);
            EXPECT_TRUE(state_controller->isInCooldown());
        } else {
            EXPECT_EQ(state_controller->getCurrentMode(), attempted_modes[0]);
            EXPECT_TRUE(state_controller->isInCooldown());
        }
        EXPECT_FALSE(state_controller->isInFallbackMode());
        EXPECT_FALSE(state_controller->isInErrorState());
    }
    
    // Verify only one mode change occurred
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), initial_history_size + 1);
    EXPECT_EQ(mode_history.back().mode, attempted_modes[0]);
    EXPECT_TRUE(mode_history.back().reason.find(attempted_reasons[0]) != std::string::npos);
    EXPECT_FALSE(mode_history.back().is_fallback);
    EXPECT_FALSE(mode_history.back().is_error);
    EXPECT_GE(mode_history.back().timestamp, decision_timestamps[0]);
    
    // Verify transition history
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), initial_transitions_size + 1);
    EXPECT_EQ(transitions.back().from, initial_mode);
    EXPECT_EQ(transitions.back().to, attempted_modes[0]);
    EXPECT_GE(transitions.back().timestamp, decision_timestamps[0]);
    
    // Verify timing between attempts
    for (size_t i = 1; i < decision_timestamps.size(); ++i) {
        auto time_between_attempts = decision_timestamps[i] - decision_timestamps[i-1];
        EXPECT_LT(time_between_attempts, StateController::kModeSwitchCooldown);
    }
    
    // Cleanup
    StateController::setForceCooldownForTesting(false);
    StateController::setDirectModeSetForTesting(false);
}

// Enhanced assertions for system fallback
TEST_F(SystemIntegrationTest, StateController_InitiatesSystemFallback_WhenDecisionEngineSignalsCriticalError) {
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // Simulate critical error in decision engine
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*gpu_source, isAvailable()).WillOnce(Return(false));
    
    // Verify error notification details
    EXPECT_CALL(*notification_service, notifyError(
        std::string("Critical error detected: all sensors unavailable")
    )).Times(1);
    
    // Verify mode change notification details
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Lean,
        std::string("Critical error detected: all sensors unavailable")
    )).Times(1);
    
    processMetrics();
    
    // Verify fallback state
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
    EXPECT_TRUE(state_controller->isInFallbackMode());
    EXPECT_TRUE(state_controller->isInErrorState());
    EXPECT_FALSE(state_controller->isInCooldown());
    
    // Verify error state details
    EXPECT_TRUE(state_controller->hasError());
    auto error_details = state_controller->getLastError();
    EXPECT_TRUE(error_details.find("Critical error detected: all sensors unavailable") != std::string::npos);
    
    // Verify mode history
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), initial_history_size + 1);
    const auto& last_entry = mode_history.back();
    EXPECT_EQ(last_entry.mode, PerformanceMode::Lean);
    EXPECT_TRUE(last_entry.is_fallback);
    EXPECT_TRUE(last_entry.is_error);
    EXPECT_TRUE(last_entry.reason.find("Critical error detected: all sensors unavailable") != std::string::npos);
    EXPECT_GE(last_entry.timestamp, std::chrono::system_clock::now() - 1s);
    
    // Verify transition history
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), initial_transitions_size + 1);
    EXPECT_EQ(transitions.back().from, initial_mode);
    EXPECT_EQ(transitions.back().to, PerformanceMode::Lean);
    EXPECT_GE(transitions.back().timestamp, std::chrono::system_clock::now() - 1s);
}

// Enhanced assertions for mode switch reason recording
TEST_F(SystemIntegrationTest, StateController_RecordsCorrectReason_ForModeSwitch) {
    struct TestCase {
        double cpu;
        double memory;
        double gpu;
        PerformanceMode expected_mode;
        std::string expected_reason;
        std::string expected_details;
    };
    
    std::vector<TestCase> test_cases = {
        {20.0, 30.0, 40.0, PerformanceMode::HighFidelity, "High performance mode activated due to high CPU usage", "low resource usage"},
        {45.0, 60.0, 75.0, PerformanceMode::Balanced, "normal", "moderate resource usage"},
        {95.0, 90.0, 85.0, PerformanceMode::Lean, "High performance mode activated due to high CPU usage", "high resource usage"}
    };
    
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    for (const auto& test_case : test_cases) {
        // Wait for cooldown if not first test
        if (&test_case != &test_cases.front()) {
            std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
        }
        
        EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(test_case.cpu));
        EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(test_case.memory));
        EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(test_case.gpu));
        
        // Verify notification details
        EXPECT_CALL(*notification_service, notifyModeChange(
            test_case.expected_mode,
            std::string(test_case.expected_reason)
        )).Times(1);
        
        processMetrics();
        
        // Verify current mode and state
        EXPECT_EQ(state_controller->getCurrentMode(), test_case.expected_mode);
        EXPECT_FALSE(state_controller->isInFallbackMode());
        EXPECT_FALSE(state_controller->isInErrorState());
        EXPECT_TRUE(state_controller->isInCooldown());
        
        // Verify mode history entry
        auto mode_history = state_controller->getModeHistory();
        ASSERT_FALSE(mode_history.empty());
        const auto& last_entry = mode_history.back();
        EXPECT_EQ(last_entry.mode, test_case.expected_mode);
        EXPECT_TRUE(last_entry.reason.find(test_case.expected_reason) != std::string::npos);
        EXPECT_TRUE(last_entry.reason.find(test_case.expected_details) != std::string::npos);
        EXPECT_FALSE(last_entry.is_fallback);
        EXPECT_FALSE(last_entry.is_error);
        EXPECT_GE(last_entry.timestamp, std::chrono::system_clock::now() - 1s);
    }
    
    // Verify final history and transition counts
    auto final_history = state_controller->getModeHistory();
    EXPECT_EQ(final_history.size(), initial_history_size + test_cases.size());
    
    auto final_transitions = state_controller->getModeTransitions();
    EXPECT_EQ(final_transitions.size(), initial_transitions_size + test_cases.size());
    
    // Verify transition sequence
    for (size_t i = 0; i < test_cases.size(); ++i) {
        const auto& transition = final_transitions[initial_transitions_size + i];
        EXPECT_EQ(transition.from, i == 0 ? initial_mode : test_cases[i-1].expected_mode);
        EXPECT_EQ(transition.to, test_cases[i].expected_mode);
    }
}

// Enhanced assertions for mode transition validation
TEST_F(SystemIntegrationTest, StateController_ValidatesModeTransitions) {
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // Set up metrics for HighFidelity mode
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::HighFidelity,
        std::string("High performance mode activated due to high CPU usage")
    )).Times(1);
    
    processMetrics();
    
    // Verify first transition
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::HighFidelity);
    EXPECT_TRUE(state_controller->isInCooldown());
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Try to switch to Lean
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(95.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(90.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(85.0));
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Lean,
        std::string("High performance mode activated due to high CPU usage")
    )).Times(1);
    
    processMetrics();
    
    // Verify second transition
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
    EXPECT_TRUE(state_controller->isInCooldown());
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    
    // Verify transition history
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), initial_transitions_size + 2);
    
    // Verify first transition
    EXPECT_EQ(transitions[initial_transitions_size].from, initial_mode);
    EXPECT_EQ(transitions[initial_transitions_size].to, PerformanceMode::HighFidelity);
    EXPECT_GE(transitions[initial_transitions_size].timestamp, std::chrono::system_clock::now() - 2s);
    
    // Verify second transition
    EXPECT_EQ(transitions[initial_transitions_size + 1].from, PerformanceMode::HighFidelity);
    EXPECT_EQ(transitions[initial_transitions_size + 1].to, PerformanceMode::Lean);
    EXPECT_GE(transitions[initial_transitions_size + 1].timestamp, std::chrono::system_clock::now() - 1s);
    
    // Verify mode history
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), initial_history_size + 2);
    
    // Verify first history entry
    EXPECT_EQ(mode_history[initial_history_size].mode, PerformanceMode::HighFidelity);
    EXPECT_TRUE(mode_history[initial_history_size].reason.find("High performance mode activated due to high CPU usage") != std::string::npos);
    EXPECT_FALSE(mode_history[initial_history_size].is_fallback);
    EXPECT_FALSE(mode_history[initial_history_size].is_error);
    
    // Verify second history entry
    EXPECT_EQ(mode_history[initial_history_size + 1].mode, PerformanceMode::Lean);
    EXPECT_TRUE(mode_history[initial_history_size + 1].reason.find("High performance mode activated due to high CPU usage") != std::string::npos);
    EXPECT_FALSE(mode_history[initial_history_size + 1].is_fallback);
    EXPECT_FALSE(mode_history[initial_history_size + 1].is_error);
}

// Enhanced assertions for handling decisions during fallback mode
TEST_F(SystemIntegrationTest, StateController_IgnoresNormalDecisions_DuringFallbackMode) {
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // First, trigger fallback mode
    EXPECT_CALL(*cpu_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*memory_source, isAvailable()).WillOnce(Return(false));
    EXPECT_CALL(*gpu_source, isAvailable()).WillOnce(Return(false));
    
    EXPECT_CALL(*notification_service, notifyError(
        std::string("Critical error detected: all sensors unavailable")
    )).Times(1);
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::Lean,
        std::string("Critical error detected: all sensors unavailable")
    )).Times(1);
    
    processMetrics();
    
    // Verify fallback state
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
    EXPECT_TRUE(state_controller->isInFallbackMode());
    EXPECT_TRUE(state_controller->isInErrorState());
    EXPECT_FALSE(state_controller->isInCooldown());
    
    // Record state before attempting normal decision
    auto fallback_history_size = state_controller->getModeHistory().size();
    auto fallback_transitions_size = state_controller->getModeTransitions().size();
    
    // Now try to switch to HighFidelity while in fallback
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    // Verify error notification details
    EXPECT_CALL(*notification_service, notifyError(
        std::string("Fallback mode activated: HighFidelity decision ignored")
    )).Times(1);
    
    // Verify no mode change notification
    EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
    
    processMetrics();
    
    // Verify state remains in fallback
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::Lean);
    EXPECT_TRUE(state_controller->isInFallbackMode());
    EXPECT_TRUE(state_controller->isInErrorState());
    EXPECT_FALSE(state_controller->isInCooldown());
    
    // Verify error state details
    EXPECT_TRUE(state_controller->hasError());
    auto error_details = state_controller->getLastError();
    EXPECT_TRUE(error_details.find("Fallback mode activated: HighFidelity decision ignored") != std::string::npos);
    
    // Verify history and transitions
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), fallback_history_size + 1);  // Error should be logged
    EXPECT_TRUE(mode_history.back().reason.find("Fallback mode activated: HighFidelity decision ignored") != std::string::npos);
    EXPECT_TRUE(mode_history.back().is_error);
    EXPECT_TRUE(mode_history.back().is_fallback);
    
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), fallback_transitions_size);  // No new transition
}

// Enhanced assertions for handling decision to current mode
TEST_F(SystemIntegrationTest, StateController_HandlesDecisionToCurrentMode) {
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // First switch to HighFidelity
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::HighFidelity,
        std::string("High performance mode activated due to high CPU usage")
    )).Times(1);
    
    processMetrics();
    
    // Verify first transition
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::HighFidelity);
    EXPECT_TRUE(state_controller->isInCooldown());
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Record state before attempting same mode
    auto current_history_size = state_controller->getModeHistory().size();
    auto current_transitions_size = state_controller->getModeTransitions().size();
    
    // Try to switch to the same mode
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    // Verify no notifications
    EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
    EXPECT_CALL(*notification_service, notifyError(_)).Times(0);
    
    processMetrics();
    
    // Verify state remains unchanged
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::HighFidelity);
    EXPECT_FALSE(state_controller->isInCooldown());
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    
    // Verify no new history or transitions
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), current_history_size);
    
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), current_transitions_size);
}

// Enhanced assertions for handling null decision
TEST_F(SystemIntegrationTest, StateController_HandlesNullDecision) {
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // Expect error notification details
    EXPECT_CALL(*notification_service, notifyError(
        std::string("Null decision rejected")
    )).Times(1);
    
    // Verify no mode change notification
    EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
    
    // Attempt to update with null decision
    state_controller->updateMode(chronovyan::ModeDecision());
    
    // Verify state remains unchanged
    EXPECT_EQ(state_controller->getCurrentMode(), initial_mode);
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    EXPECT_FALSE(state_controller->isInCooldown());
    
    // Verify error state details
    EXPECT_TRUE(state_controller->hasError());
    auto error_details = state_controller->getLastError();
    EXPECT_TRUE(error_details.find("Null decision rejected") != std::string::npos);
    
    // Verify history and transitions
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), initial_history_size + 1);  // Error should be logged
    EXPECT_TRUE(mode_history.back().reason.find("Null decision rejected") != std::string::npos);
    EXPECT_TRUE(mode_history.back().is_error);
    EXPECT_FALSE(mode_history.back().is_fallback);
    
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), initial_transitions_size);  // No new transition
}

// Enhanced assertions for handling decision during error state
TEST_F(SystemIntegrationTest, StateController_HandlesDecisionDuringErrorState) {
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // First, trigger an error state (but not fallback)
    EXPECT_CALL(*cpu_source, getValue())
        .WillOnce(Throw(std::runtime_error("Sensor error")));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(60.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(75.0));
    
    EXPECT_CALL(*notification_service, notifyError(
        std::string("Critical error detected: sensor error")
    )).Times(1);
    
    processMetrics();
    
    // Verify error state
    EXPECT_EQ(state_controller->getCurrentMode(), initial_mode);
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_TRUE(state_controller->isInErrorState());
    EXPECT_TRUE(state_controller->isInCooldown());
    
    // Verify error details
    EXPECT_TRUE(state_controller->hasError());
    auto error_details = state_controller->getLastError();
    EXPECT_TRUE(error_details.find("Critical error detected: sensor error") != std::string::npos);
    
    // Wait for cooldown
    std::this_thread::sleep_for(StateController::kModeSwitchCooldown + 1s);
    
    // Try to switch to HighFidelity
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    EXPECT_CALL(*notification_service, notifyModeChange(
        PerformanceMode::HighFidelity,
        std::string("Recovered from error state")
    )).Times(1);
    
    processMetrics();
    
    // Verify state transition
    EXPECT_EQ(state_controller->getCurrentMode(), PerformanceMode::HighFidelity);
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    EXPECT_TRUE(state_controller->isInCooldown());
    
    // Verify error state cleared
    EXPECT_FALSE(state_controller->hasError());
    
    // Verify history and transitions
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), initial_history_size + 2);  // Error + recovery
    
    // Verify error entry
    EXPECT_TRUE(mode_history[initial_history_size].reason.find("Recovered from error state") != std::string::npos);
    EXPECT_TRUE(mode_history[initial_history_size].is_error);
    EXPECT_FALSE(mode_history[initial_history_size].is_fallback);
    
    // Verify recovery entry
    EXPECT_EQ(mode_history[initial_history_size + 1].mode, PerformanceMode::HighFidelity);
    EXPECT_TRUE(mode_history[initial_history_size + 1].reason.find("Recovered from error state") != std::string::npos);
    EXPECT_FALSE(mode_history[initial_history_size + 1].is_error);
    EXPECT_FALSE(mode_history[initial_history_size + 1].is_fallback);
    
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), initial_transitions_size + 1);
    EXPECT_EQ(transitions.back().from, initial_mode);
    EXPECT_EQ(transitions.back().to, PerformanceMode::HighFidelity);
}

// Enhanced assertions for handling decision with invalid reason
TEST_F(SystemIntegrationTest, StateController_HandlesDecisionWithInvalidReason) {
    // Record initial state
    auto initial_mode = state_controller->getCurrentMode();
    auto initial_history_size = state_controller->getModeHistory().size();
    auto initial_transitions_size = state_controller->getModeTransitions().size();
    
    // Set up metrics for HighFidelity mode
    EXPECT_CALL(*cpu_source, getValue()).WillOnce(Return(20.0));
    EXPECT_CALL(*memory_source, getValue()).WillOnce(Return(30.0));
    EXPECT_CALL(*gpu_source, getValue()).WillOnce(Return(40.0));
    
    // Create a decision with an invalid reason
    auto metrics = collector->collect_metrics();
    auto decision = decision_engine->evaluate_metrics(metrics);
    decision.reason = "";  // Empty reason
    
    // Expect error notification details
    EXPECT_CALL(*notification_service, notifyError(
        std::string("Invalid reason rejected")
    )).Times(1);
    
    // Verify no mode change notification
    EXPECT_CALL(*notification_service, notifyModeChange(_, _)).Times(0);
    
    // Attempt mode switch
    state_controller->updateMode(decision);
    
    // Verify state remains unchanged
    EXPECT_EQ(state_controller->getCurrentMode(), initial_mode);
    EXPECT_FALSE(state_controller->isInFallbackMode());
    EXPECT_FALSE(state_controller->isInErrorState());
    EXPECT_FALSE(state_controller->isInCooldown());
    
    // Verify error state details
    EXPECT_TRUE(state_controller->hasError());
    auto error_details = state_controller->getLastError();
    EXPECT_TRUE(error_details.find("Invalid reason rejected") != std::string::npos);
    
    // Verify history and transitions
    auto mode_history = state_controller->getModeHistory();
    EXPECT_EQ(mode_history.size(), initial_history_size + 1);  // Error should be logged
    EXPECT_TRUE(mode_history.back().reason.find("Invalid reason rejected") != std::string::npos);
    EXPECT_TRUE(mode_history.back().is_error);
    EXPECT_FALSE(mode_history.back().is_fallback);
    
    auto transitions = state_controller->getModeTransitions();
    EXPECT_EQ(transitions.size(), initial_transitions_size);  // No new transition
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 