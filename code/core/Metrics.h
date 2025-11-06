#pragma once
#include <vector>
class City; class Agent;

class Metrics {
public:
    void recordDeparture(const Agent& a);
    void recordArrival(const Agent& a, int timeSteps);
    void snapshotEdgeLoads(const City& city);
    double averageTripTime() const;
    int totalThroughput() const;
    int getMaxEdgeLoad() const;
    void updateMaxEdgeLoad(int load);  // Update max edge load if higher
    void tick();
    void reset();
    
    // Getters for testing
    int getCurrentTick() const { return currentTick_; }
    const std::vector<double>& getTripTimes() const { return tripTimes_; }
    const std::vector<int>& getThroughputPerTick() const { return throughputPerTick_; }
    const std::vector<std::vector<int>>& getEdgeLoadHistory() const { return edgeLoadHistory_; }
    
private:
    std::vector<double> tripTimes_;
    std::vector<int> throughputPerTick_;
    std::vector<std::vector<int>> edgeLoadHistory_;  // Per-tick edge loads
    int maxEdgeLoad_ = 0;
    int currentTick_ = 0;
};
