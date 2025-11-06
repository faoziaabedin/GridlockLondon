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
private:
    std::vector<double> tripTimes_;
    std::vector<int> throughputPerTick_;
    int maxEdgeLoad_ = 0;
    int currentTick_ = 0;
};
