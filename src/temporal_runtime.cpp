#include "temporal_runtime.h"
#include "error_handler.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace chronovyan {

// Timeline implementation

Timeline::Timeline(size_t id, std::string name, std::shared_ptr<Environment> environment)
    : m_id(id), m_name(std::move(name)), m_environment(std::move(environment)) {}

size_t Timeline::getId() const {
    return m_id;
}

const std::string& Timeline::getName() const {
    return m_name;
}

std::shared_ptr<Environment> Timeline::getEnvironment() const {
    return m_environment;
}

TimePoint Timeline::createTimePoint(const std::string& label) {
    TimePoint point(label, m_nextTimePointId++);
    m_timePoints.push_back(point);
    return point;
}

const TimePoint& Timeline::getTimePoint(size_t id) const {
    auto it = std::find_if(m_timePoints.begin(), m_timePoints.end(),
                         [id](const TimePoint& tp) { return tp.id == id; });
    
    if (it == m_timePoints.end()) {
        throw std::out_of_range("Time point does not exist");
    }
    
    return *it;
}

void Timeline::saveState(const TimePoint& timePoint) {
    // Create a copy of the current environment
    auto envCopy = std::make_shared<Environment>(*m_environment);
    
    // Store it associated with the time point
    m_savedStates[timePoint.id] = std::move(envCopy);
}

void Timeline::restoreState(const TimePoint& timePoint) {
    auto it = m_savedStates.find(timePoint.id);
    if (it == m_savedStates.end()) {
        throw std::out_of_range("No saved state for this time point");
    }
    
    // Restore the environment
    m_environment = std::make_shared<Environment>(*it->second);
}

std::shared_ptr<Timeline> Timeline::clone(size_t newId, const std::string& newName) const {
    // Create a new timeline with a copy of the environment
    auto envCopy = std::make_shared<Environment>(*m_environment);
    return std::make_shared<Timeline>(newId, newName, std::move(envCopy));
}

// TemporalRuntime implementation

TemporalRuntime::TemporalRuntime(std::shared_ptr<Environment> globalEnvironment) {
    // Create the initial timeline
    m_currentTimeline = createTimeline("Main", std::move(globalEnvironment));
}

std::shared_ptr<Timeline> TemporalRuntime::getCurrentTimeline() const {
    return m_currentTimeline;
}

std::shared_ptr<Timeline> TemporalRuntime::getTimeline(size_t id) const {
    auto it = std::find_if(m_timelines.begin(), m_timelines.end(),
                         [id](const std::shared_ptr<Timeline>& t) { return t->getId() == id; });
    
    if (it == m_timelines.end()) {
        throw std::out_of_range("Timeline does not exist");
    }
    
    return *it;
}

const std::vector<std::shared_ptr<Timeline>>& TemporalRuntime::getTimelines() const {
    return m_timelines;
}

TimePoint TemporalRuntime::createTimePoint(const std::string& label) {
    return m_currentTimeline->createTimePoint(label);
}

std::shared_ptr<Timeline> TemporalRuntime::branchTimeline(const std::string& name) {
    // Clone the current timeline to create a branch
    auto branch = m_currentTimeline->clone(m_nextTimelineId++, name);
    
    // Add the new timeline to the collection
    m_timelines.push_back(branch);
    
    // Increase paradox level when branching
    increaseParadoxLevel(0.5);
    
    return branch;
}

void TemporalRuntime::mergeTimeline(size_t timelineId) {
    auto timeline = getTimeline(timelineId);
    
    // Simple merge: just copy all variables from the other timeline
    // to the current timeline. In a real implementation, this would
    // be more sophisticated.
    
    // For now, just increase paradox level
    increaseParadoxLevel(1.0);
    
    // Remove the merged timeline
    m_timelines.erase(
        std::remove_if(m_timelines.begin(), m_timelines.end(),
                     [timelineId](const std::shared_ptr<Timeline>& t) { return t->getId() == timelineId; }),
        m_timelines.end()
    );
}

void TemporalRuntime::rewindTo(const TimePoint& timePoint) {
    try {
        // Save the current state if needed
        TimePoint currentPoint = createTimePoint("Before rewind");
        m_currentTimeline->saveState(currentPoint);
        
        // Rewind to the specified time point
        m_currentTimeline->restoreState(timePoint);
        
        // Increase paradox level when rewinding
        increaseParadoxLevel(0.3);
    } catch (const std::out_of_range& e) {
        throw ChronovyanRuntimeError(e.what(), SourceLocation());
    }
}

void TemporalRuntime::switchTimeline(size_t timelineId) {
    try {
        auto timeline = getTimeline(timelineId);
        m_currentTimeline = timeline;
        
        // Increase paradox level when switching timelines
        increaseParadoxLevel(0.3);
    } catch (const std::out_of_range& e) {
        throw ChronovyanRuntimeError(e.what(), SourceLocation());
    }
}

double TemporalRuntime::getParadoxLevel() const {
    return m_paradoxLevel;
}

void TemporalRuntime::increaseParadoxLevel(double amount) {
    m_paradoxLevel += amount;
    
    // Cap the paradox level
    if (m_paradoxLevel > 10.0) {
        m_paradoxLevel = 10.0;
        
        // Apply effects when paradox level is high
        applyParadoxEffects();
    }
}

void TemporalRuntime::decreaseParadoxLevel(double amount) {
    m_paradoxLevel -= amount;
    
    if (m_paradoxLevel < 0.0) {
        m_paradoxLevel = 0.0;
    }
}

double TemporalRuntime::getAethelLevel() const {
    return m_aethelLevel;
}

void TemporalRuntime::consumeAethel(double amount) {
    if (m_aethelLevel < amount) {
        throw ChronovyanRuntimeError("Not enough Aethel resources", SourceLocation());
    }
    
    m_aethelLevel -= amount;
}

void TemporalRuntime::regenerateAethel(double amount) {
    m_aethelLevel += amount;
    
    // Cap the Aethel level
    if (m_aethelLevel > 100.0) {
        m_aethelLevel = 100.0;
    }
}

double TemporalRuntime::getChrononsLevel() const {
    return m_chrononsLevel;
}

void TemporalRuntime::consumeChronons(double amount) {
    if (m_chrononsLevel < amount) {
        throw ChronovyanRuntimeError("Not enough Chronons resources", SourceLocation());
    }
    
    m_chrononsLevel -= amount;
}

void TemporalRuntime::regenerateChronons(double amount) {
    m_chrononsLevel += amount;
    
    // Cap the Chronons level
    if (m_chrononsLevel > 100.0) {
        m_chrononsLevel = 100.0;
    }
}

std::shared_ptr<Timeline> TemporalRuntime::createTimeline(const std::string& name, std::shared_ptr<Environment> environment) {
    auto timeline = std::make_shared<Timeline>(m_nextTimelineId++, name, environment);
    m_timelines.push_back(timeline);
    return timeline;
}

void TemporalRuntime::applyParadoxEffects() {
    // In a real implementation, this would apply various effects based on paradox level
    // For example, causing quantum fluctuations, timeline instability, etc.
}

} // namespace chronovyan 