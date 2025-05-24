#pragma once

#include "chronovyan/performance_mode.hpp"
#include "chronovyan/mode_decision_engine.hpp"
#include "chronovyan/notification_service.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace chronovyan {

/**
 * Central state controller that manages performance mode transitions
 * and enforces rules like cooldown periods between mode changes.
 */
class StateController {
public:
    // Default cooldown period between mode changes (5 seconds)
    static constexpr std::chrono::milliseconds kModeSwitchCooldown = std::chrono::milliseconds(5000);
    
    // Maximum history entries to store
    static constexpr size_t kMaxHistoryEntries = 50;
    
    /**
     * Constructor.
     * 
     * @param decision_engine The engine that makes mode decisions based on metrics
     * @param notification_service Service to notify about mode changes and errors
     */
    StateController(std::shared_ptr<ModeDecisionEngine> decision_engine,
                  std::shared_ptr<INotificationService> notification_service);
    
    /**
     * Handles a mode update based on a decision.
     * May reject the update if in cooldown or error state.
     * 
     * @param decision The mode decision to apply
     */
    void updateMode(const ModeDecision& decision);
    
    // Static methods for testing
    static void setForceCooldownForTesting(bool force_cooldown);
    static bool getForceCooldownForTesting();
    static void setDirectModeSetForTesting(bool enable);
    
    /**
     * New method to bypass cooldown for a single mode update operation.
     * This will be reset after the next updateMode call.
     * 
     * @param bypass Whether to bypass cooldown for the next mode update
     */
    void setBypassCooldownForNextUpdate(bool bypass);
    
    // State accessors
    PerformanceMode getCurrentMode() const;
    bool isInFallbackMode() const;
    bool isInErrorState() const;
    bool isInCooldown() const;
    std::string getErrorDetails() const;
    std::chrono::milliseconds timeUntilNextSwitch() const;
    
    // History and transitions accessors
    const std::vector<ModeDecision>& getModeHistory() const;
    const std::vector<std::pair<PerformanceMode, std::string>>& getTransitionHistory() const;
    
private:
    // Check if a mode switch can occur
    bool canSwitchMode() const;
    
    // Handle error state 
    void handleErrorState(const ModeDecision& decision, 
                          std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());
    
    // Handle fallback mode
    void handleFallbackMode(const ModeDecision& decision,
                           std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());
    
    // Record a mode change with timestamp
    void recordModeChange(const ModeDecision& decision, 
                         PerformanceMode old_mode,
                         std::chrono::system_clock::time_point timestamp);
    
    // Current state
    PerformanceMode current_mode_;
    bool is_in_fallback_mode_;
    bool is_in_error_state_;
    bool is_in_cooldown_;
    std::string error_details_;
    std::chrono::system_clock::time_point last_update_time_;
    
    // Bypass cooldown flag for a single update operation
    bool bypass_cooldown_for_mode_switch_ = false;
    
    // History
    std::vector<ModeDecision> mode_decision_history_;
    std::vector<std::pair<PerformanceMode, std::string>> mode_transition_history_;
    
    // Services
    std::shared_ptr<INotificationService> notification_service_;
    
    // Static state for testing
    static bool force_cooldown_for_testing_;
    static bool is_direct_mode_set_;
};

} // namespace chronovyan 