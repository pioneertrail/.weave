#pragma once

#include "chronovyan/common_types.hpp"
#include "chronovyan/notification_service.hpp"
#include "chronovyan/mode_decision_engine.hpp"
#include <chrono>
#include <deque>
#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace chronovyan {

/**
 * @brief Controller for managing system performance mode transitions.
 * 
 * This class handles transitions between different performance modes
 * based on decisions from the ModeDecisionEngine. It enforces cooldown periods,
 * validates transitions, and manages error and fallback states.
 */
class StateController {
public:
    // Constants
    static constexpr std::chrono::milliseconds kModeSwitchCooldown{1000};  // Minimum time between mode switches
    static constexpr size_t kMaxHistoryEntries = 50;                // Maximum number of history entries to keep
    static constexpr size_t kMaxTransitionEntries = 50;             // Maximum number of transition entries to keep

    // Constructor and main methods
    StateController(
        std::shared_ptr<ModeDecisionEngine> decision_engine,
        std::shared_ptr<INotificationService> notification_service);
    void updateMode(const ModeDecision& decision);
    
    // State accessors
    PerformanceMode getCurrentMode() const;
    bool isInFallbackMode() const;
    bool isInErrorState() const;
    bool isInCooldown() const;
    
    // Cooldown-related methods
    std::chrono::milliseconds timeUntilNextSwitch() const;
    std::chrono::system_clock::time_point getCooldownEndTime() const {
        return last_update_time_ + kModeSwitchCooldown;
    }
    
    /**
     * Bypass cooldown for the next mode update call.
     * This flag will be reset after updateMode is called.
     * 
     * @param bypass Whether to bypass cooldown for the next update
     */
    void setBypassCooldownForNextUpdate(bool bypass);
    
    // Error-related methods
    std::string getErrorDetails() const;
    
    // History and transitions accessors
    const std::vector<ModeDecision>& getModeHistory() const;
    const std::vector<std::pair<PerformanceMode, std::string>>& getTransitionHistory() const;
    
    // Static methods for testing
    static void setForceCooldownForTesting(bool force_cooldown);
    static bool getForceCooldownForTesting();
    static void setDirectModeSetForTesting(bool enable);
    
    // Additional testing method to directly set the mode
    void setCurrentModeForTesting(PerformanceMode mode) {
        current_mode_ = mode;
    }

    // Additional method to manipulate mode history for testing
    void addModeDecisionForTesting(const ModeDecision& decision) {
        mode_decision_history_.push_back(decision);
    }

    // Additional method to manipulate transitions for testing
    void addTransitionForTesting(PerformanceMode from_mode, const std::string& reason) {
        mode_transition_history_.emplace_back(from_mode, reason);
    }

    // Additional test methods for direct access to history
    std::vector<ModeDecision>& getModeHistoryForTesting() { return mode_decision_history_; }
    std::vector<std::pair<PerformanceMode, std::string>>& getTransitionHistoryForTesting() { return mode_transition_history_; }

    // Public static variables for testing
    static bool is_direct_mode_set_; ///< Flag to bypass normal checks for direct mode setting

private:
    // State
    PerformanceMode current_mode_ = PerformanceMode::Balanced;
    bool is_in_fallback_mode_ = false;
    bool is_in_error_state_ = false;
    bool is_in_cooldown_ = false;
    std::string error_details_;
    
    // Flag to bypass cooldown for a single update
    bool bypass_cooldown_for_mode_switch_ = false;
    
    // Time of the last mode change (for cooldown)
    std::chrono::system_clock::time_point last_update_time_;
    
    // History of mode changes
    std::vector<ModeDecision> mode_decision_history_;
    
    // History of mode transitions
    std::vector<std::pair<PerformanceMode, std::string>> mode_transition_history_;
    
    // Notification service for callbacks
    std::shared_ptr<INotificationService> notification_service_;
    
    // Helper methods
    bool canSwitchMode() const;
    void recordModeChange(const ModeDecision& decision, PerformanceMode old_mode, std::chrono::system_clock::time_point timestamp, bool bypass_cooldown = false);
    void handleErrorState(const ModeDecision& decision, std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now(), bool bypass_cooldown = false);
    void handleFallbackMode(const ModeDecision& decision, std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now(), bool bypass_cooldown = false);
    
    // Static members for testing
    static bool force_cooldown_for_testing_;
    
    // Member variables for tracking error states
    std::chrono::system_clock::time_point partial_sensor_failure_time_;

    // Helper method to validate performance mode values
    bool isValidPerformanceMode(PerformanceMode mode) const;
};

} // namespace chronovyan 