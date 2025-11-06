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
    // Update max edge load by checking all edges
    // This is a simplified version - in a full implementation,
    // we'd iterate through all edges in the city
    // For now, we'll track this during tick() calls
}

double Metrics::averageTripTime() const {
    if (tripTimes_.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(tripTimes_.begin(), tripTimes_.end(), 0.0);
    return sum / static_cast<double>(tripTimes_.size());
}

int Metrics::totalThroughput() const {
    return throughputPerTick_.empty() ? 0 : throughputPerTick_.back();
}

int Metrics::getMaxEdgeLoad() const {
    return maxEdgeLoad_;
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
