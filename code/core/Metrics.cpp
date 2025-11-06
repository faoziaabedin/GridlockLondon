// code/core/Metrics.cpp
#include "Metrics.h"
#include "City.h"
#include "Agent.h"
#include <algorithm>
#include <numeric>

void Metrics::recordDeparture(const Agent& a) {
    // Record when an agent departs
    // Track departure time for potential future use
    // For now, we track this implicitly through recordArrival()
}

void Metrics::recordArrival(const Agent& a, int timeSteps) {
    tripTimes_.push_back(static_cast<double>(timeSteps));
    
    // Update throughput for current tick
    if (!throughputPerTick_.empty()) {
        throughputPerTick_.back()++;
    } else {
        // If no tick has been called yet, start tracking
        throughputPerTick_.push_back(1);
    }
}

void Metrics::snapshotEdgeLoads(const City& city) {
    // Capture edge loads for this tick
    // Since City doesn't expose edge iteration directly, we track max load
    // during agent movement. This method is called after all agents have moved.
    // In a full implementation with edge iteration, we would capture all edge loads here.
    
    // For now, we maintain edgeLoadHistory_ structure but populate it elsewhere
    // The max edge load is updated via updateMaxEdgeLoad() during agent movement
    if (edgeLoadHistory_.empty()) {
        edgeLoadHistory_.push_back(std::vector<int>());
    }
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
    // Initialize throughput for this tick (will be updated when agents arrive)
    throughputPerTick_.push_back(0);
    // Initialize edge load history for this tick
    edgeLoadHistory_.push_back(std::vector<int>());
}

void Metrics::reset() {
    tripTimes_.clear();
    throughputPerTick_.clear();
    edgeLoadHistory_.clear();
    maxEdgeLoad_ = 0;
    currentTick_ = 0;
}
