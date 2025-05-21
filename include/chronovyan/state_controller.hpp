#pragma once

#include "chronovyan/common_types.hpp"
#include "chronovyan/notification_service.hpp"
#include "chronovyan/mode_decision_engine.hpp"
#include <chrono>
#include <deque>
#include <string>
#include <vector>

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
    static constexpr std::chrono::seconds kModeSwitchCooldown{10};  // Minimum time between mode switches
    static constexpr size_t kMaxHistoryEntries = 50;                // Maximum number of history entries to keep
    static constexpr size_t kMaxTransitionEntries = 50;             // Maximum number of transition entries to keep

    // History and transition entry types
    struct ModeHistoryEntry {
        PerformanceMode mode;
        std::string reason;
        std::chrono::system_clock::time_point timestamp;
        bool is_error;
        bool is_fallback;
    };

    struct ModeTransition {
        PerformanceMode from;
        PerformanceMode to;
        std::chrono::system_clock::time_point timestamp;
    };
    
    // Type aliases for history and transition collections
    using ModeHistoryList = std::deque<ModeHistoryEntry>;
    using ModeTransitionList = std::deque<ModeTransition>;

    // Constructor and main methods
    explicit StateController(INotificationService* notification_service);
    void updateMode(const ModeDecision& decision);
    
    // State accessors
    PerformanceMode getCurrentMode() const { return current_mode_; }
    bool isInFallbackMode() const { return is_in_fallback_mode_; }
    bool isInErrorState() const { return is_in_error_state_; }
    bool isInCooldown() const;
    
    // Cooldown-related methods
    std::chrono::seconds getTimeUntilNextSwitch() const;
    std::chrono::system_clock::time_point getCooldownEndTime() const {
        return last_mode_change_time_ + kModeSwitchCooldown;
    }
    
    // Error-related methods
    bool hasError() const { return !last_error_.empty(); }
    std::string getLastError() const { return last_error_; }
    
    // History and transitions accessors
    const ModeHistoryList& getModeHistory() const { return mode_history_; }
    const ModeTransitionList& getModeTransitions() const { return mode_transitions_; }
    
    // Static methods for testing
    static void setForceCooldownForTesting(bool force_cooldown);
    static bool getForceCooldownForTesting();
    static void setDirectModeSetForTesting(bool direct_set);
    
private:
    // State
    PerformanceMode current_mode_ = PerformanceMode::Balanced;
    bool is_in_fallback_mode_ = false;
    bool is_in_error_state_ = false;
    bool is_in_cooldown_ = false;
    std::string last_error_;
    
    // Time of the last mode change (for cooldown)
    std::chrono::system_clock::time_point last_mode_change_time_;
    
    // History of mode changes
    ModeHistoryList mode_history_;
    
    // History of mode transitions
    ModeTransitionList mode_transitions_;
    
    // Notification service for callbacks
    INotificationService* notification_service_;
    
    // Helper methods
    bool canSwitchMode(PerformanceMode requested_mode) const;
    void recordModeChange(const ModeDecision& decision, PerformanceMode old_mode);
    void handleErrorState(const ModeDecision& decision);
    void handleFallbackMode(const ModeDecision& decision);
    
    // Static members for testing
    static bool force_cooldown_for_testing_;
    static bool direct_set_for_testing_;
};

} // namespace chronovyan 