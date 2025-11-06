// code/core/Metrics.h
#pragma once
#include <vector>
class City; class Agent;

/**
 * Metrics captures and calculates simulation KPIs.
 * Tracks trip times, throughput, and edge loads.
 */
class Metrics {
public:
    /**
     * Record when an agent departs.
     * @param a Agent that is departing
     */
    void recordDeparture(const Agent& a);
    
    /**
     * Record when an agent arrives at its destination.
     * @param a Agent that has arrived
     * @param timeSteps Number of time steps taken for the trip
     */
    void recordArrival(const Agent& a, int timeSteps);
    
    /**
     * Capture current edge loads for this tick.
     * @param city Reference to the city to snapshot
     */
    void snapshotEdgeLoads(const City& city);
    
    /**
     * Calculate average trip time.
     * @return Average trip time in ticks, or 0.0 if no trips completed
     */
    double averageTripTime() const;
    
    /**
     * Get total number of completed trips.
     * @return Total throughput (number of agents that have arrived)
     */
    int totalThroughput() const;
    
    /**
     * Get maximum edge load observed across the simulation.
     * @return Maximum occupancy seen on any edge
     */
    int getMaxEdgeLoad() const;
    
    /**
     * Update max edge load if the given load is higher.
     * @param load Current edge load to check
     */
    void updateMaxEdgeLoad(int load);
    
    /**
     * Increment the tick counter and initialize per-tick data.
     */
    void tick();
    
    /**
     * Reset all metrics to initial state.
     */
    void reset();
    
    // Getters for testing
    int getCurrentTick() const { return currentTick_; }
    const std::vector<double>& getTripTimes() const { return tripTimes_; }
    const std::vector<int>& getThroughputPerTick() const { return throughputPerTick_; }
    const std::vector<std::vector<int>>& getEdgeLoadHistory() const { return edgeLoadHistory_; }

private:
    std::vector<double> tripTimes_;              // Trip times for completed trips
    std::vector<int> throughputPerTick_;        // Throughput per tick
    std::vector<std::vector<int>> edgeLoadHistory_;  // Per-tick edge loads
    int maxEdgeLoad_ = 0;                        // Maximum edge load observed
    int currentTick_ = 0;                        // Current simulation tick
};
