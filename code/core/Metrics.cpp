// code/core/Metrics.cpp
#include "Metrics.h"
#include "City.h"
#include "Agent.h"
#include <algorithm>
#include <numeric>

void Metrics::recordDeparture(const Agent& a) {
    // Record when an agent departs
    // This can be used for tracking departure times if needed
}

void Metrics::recordArrival(const Agent& a, int timeSteps) {
    tripTimes_.push_back(static_cast<double>(timeSteps));
}

void Metrics::snapshotEdgeLoads(const City& city) {
    // Max edge load is updated during agent movement in SimulationController::tick()
    // This method is called after all agents have moved, so maxEdgeLoad_ should
    // already be updated. In a full implementation, we could iterate all edges here
    // to ensure we capture the maximum load across all edges.
}

double Metrics::averageTripTime() const {
    if (tripTimes_.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(tripTimes_.begin(), tripTimes_.end(), 0.0);
    return sum / static_cast<double>(tripTimes_.size());
}

int Metrics::totalThroughput() const {
    // Total throughput is the total number of agents that have arrived
    return static_cast<int>(tripTimes_.size());
}

int Metrics::getMaxEdgeLoad() const {
    return maxEdgeLoad_;
}

void Metrics::updateMaxEdgeLoad(int load) {
    if (load > maxEdgeLoad_) {
        maxEdgeLoad_ = load;
    }
}

void Metrics::tick() {
    currentTick_++;
    // Reset throughput for this tick (will be updated when agents arrive)
    throughputPerTick_.push_back(0);
}

void Metrics::reset() {
    tripTimes_.clear();
    throughputPerTick_.clear();
    maxEdgeLoad_ = 0;
    currentTick_ = 0;
}
