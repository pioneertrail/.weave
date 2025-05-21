#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include "chronovyan/quantum.hpp"

namespace chronovyan {
namespace sync {

// Timeline represents a sequence of temporal events
class Timeline {
public:
    Timeline() = default;
    ~Timeline() = default;
    
    // Prevent copying
    Timeline(const Timeline&) = delete;
    Timeline& operator=(const Timeline&) = delete;
    
    // Add a synchronization point to the timeline
    void add_sync_point(const SyncPoint& point);
    
    // Get the current sync point
    SyncPoint get_current_sync_point() const;
    
    // Get the most recent sync points, up to 'count'
    std::vector<SyncPoint> get_recent_sync_points(size_t count) const;
    
    // Clear all sync points
    void clear();
    
    // Get the number of sync points in the timeline
    size_t size() const;
    
    // Check if the timeline is empty
    bool empty() const;
    
private:
    std::vector<SyncPoint> sync_points_;
};

// Timeline event types
enum class TimelineEventType {
    SyncPoint,
    Pattern,
    Error,
    Recovery,
    Optimization
};

// Timeline event structure
struct TimelineEvent {
    TimelineEventType type;
    std::chrono::system_clock::time_point timestamp;
    std::string description;
    int sequence_id;
    double importance;
    
    TimelineEvent() = default;
    TimelineEvent(TimelineEventType t, 
                 std::chrono::system_clock::time_point ts,
                 const std::string& desc,
                 int seq = 0,
                 double imp = 1.0)
        : type(t), timestamp(ts), description(desc), sequence_id(seq), importance(imp) {}
};

// Timeline manager - manages multiple timelines
class TimelineManager {
public:
    TimelineManager() = default;
    ~TimelineManager() = default;
    
    // Prevent copying
    TimelineManager(const TimelineManager&) = delete;
    TimelineManager& operator=(const TimelineManager&) = delete;
    
    // Get a timeline by name (creates it if it doesn't exist)
    Timeline& get_timeline(const std::string& name);
    
    // Check if a timeline exists
    bool has_timeline(const std::string& name) const;
    
    // Remove a timeline
    void remove_timeline(const std::string& name);
    
    // Add an event to the global event log
    void add_event(const TimelineEvent& event);
    
    // Get recent events from the global log, up to 'count'
    std::vector<TimelineEvent> get_recent_events(size_t count) const;
    
private:
    std::map<std::string, Timeline> timelines_;
    std::vector<TimelineEvent> event_log_;
};

} // namespace sync
} // namespace chronovyan 