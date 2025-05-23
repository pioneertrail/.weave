#ifndef CHRONOVYAN_TEMPORAL_RUNTIME_H
#define CHRONOVYAN_TEMPORAL_RUNTIME_H

#include <memory>
#include <string>
#include <map>
#include <vector>

namespace chronovyan {

/**
 * @class TemporalRuntime
 * @brief Manages the temporal aspects of the Chronovyan language
 */
class TemporalRuntime {
public:
    TemporalRuntime();
    ~TemporalRuntime();

    // Paradox and timeline management
    int getParadoxLevel() const;
    void increaseParadoxLevel(int amount = 1);
    void decreaseParadoxLevel(int amount = 1);
    
    // Resource management
    double getAethelLevel() const;
    void consumeAethel(double amount);
    void replenishAethel(double amount);
    
    double getChrononsLevel() const;
    void consumeChronons(double amount);
    void replenishChronons(double amount);
    
    // Timeline operations
    void createTimelineSnapshot();
    void rewindToSnapshot(const std::string& snapshotId);
    void mergeTimelines(const std::vector<std::string>& timelineIds);
    
private:
    int m_paradoxLevel;
    double m_aethelLevel;
    double m_chrononsLevel;
    
    // Timeline snapshot storage
    std::map<std::string, std::shared_ptr<void>> m_timelineSnapshots;
};

} // namespace chronovyan

#endif // CHRONOVYAN_TEMPORAL_RUNTIME_H 