// code/tests/test_metrics.cpp
// Test program for Metrics implementation
#include <iostream>
#include <cassert>
#include <cmath>
#include "core/Metrics.h"
#include "core/Agent.h"
#include "core/City.h"
#include "core/Node.h"
#include "core/Edge.h"

void testInitialState() {
    std::cout << "Test: Initial State\n";
    
    Metrics metrics;
    
    assert(metrics.getTripTimes().empty());
    assert(metrics.getThroughputPerTick().empty());
    assert(metrics.getEdgeLoadHistory().empty());
    assert(metrics.getMaxEdgeLoad() == 0);
    assert(metrics.getCurrentTick() == 0);
    assert(metrics.averageTripTime() == 0.0);
    assert(metrics.totalThroughput() == 0);
    
    std::cout << "✓ Metrics initialized with correct default values\n";
}

void testRecordDeparture() {
    std::cout << "\nTest: Record Departure\n";
    
    Metrics metrics;
    Agent agent(1, 0, 5);
    
    // Record departure should not crash
    metrics.recordDeparture(agent);
    
    // Should still be at tick 0
    assert(metrics.getCurrentTick() == 0);
    
    std::cout << "✓ recordDeparture() executes without error\n";
}

void testRecordArrival() {
    std::cout << "\nTest: Record Arrival\n";
    
    Metrics metrics;
    Agent agent1(1, 0, 5);
    Agent agent2(2, 1, 6);
    
    // Record first arrival
    metrics.recordArrival(agent1, 10);
    
    assert(metrics.getTripTimes().size() == 1);
    assert(std::abs(metrics.getTripTimes()[0] - 10.0) < 0.001);
    assert(metrics.totalThroughput() == 1);
    
    // Record second arrival
    metrics.recordArrival(agent2, 15);
    
    assert(metrics.getTripTimes().size() == 2);
    assert(std::abs(metrics.getTripTimes()[1] - 15.0) < 0.001);
    assert(metrics.totalThroughput() == 2);
    
    std::cout << "✓ Arrivals recorded correctly\n";
    std::cout << "✓ Trip times stored: " << metrics.getTripTimes()[0] 
              << ", " << metrics.getTripTimes()[1] << "\n";
}

void testAverageTripTime() {
    std::cout << "\nTest: Average Trip Time Calculation\n";
    
    Metrics metrics;
    
    // Empty should return 0
    assert(metrics.averageTripTime() == 0.0);
    
    // Single trip
    Agent agent1(1, 0, 5);
    metrics.recordArrival(agent1, 10);
    assert(std::abs(metrics.averageTripTime() - 10.0) < 0.001);
    
    // Multiple trips
    Agent agent2(2, 1, 6);
    metrics.recordArrival(agent2, 20);
    // Average: (10 + 20) / 2 = 15
    assert(std::abs(metrics.averageTripTime() - 15.0) < 0.001);
    
    // More trips
    Agent agent3(3, 2, 7);
    metrics.recordArrival(agent3, 30);
    // Average: (10 + 20 + 30) / 3 = 20
    assert(std::abs(metrics.averageTripTime() - 20.0) < 0.001);
    
    std::cout << "✓ Average trip time calculated correctly: " 
              << metrics.averageTripTime() << "\n";
}

void testTotalThroughput() {
    std::cout << "\nTest: Total Throughput\n";
    
    Metrics metrics;
    assert(metrics.totalThroughput() == 0);
    
    Agent agent1(1, 0, 5);
    metrics.recordArrival(agent1, 10);
    assert(metrics.totalThroughput() == 1);
    
    Agent agent2(2, 1, 6);
    metrics.recordArrival(agent2, 15);
    assert(metrics.totalThroughput() == 2);
    
    std::cout << "✓ Total throughput calculated correctly: " 
              << metrics.totalThroughput() << "\n";
}

void testMaxEdgeLoad() {
    std::cout << "\nTest: Max Edge Load\n";
    
    Metrics metrics;
    assert(metrics.getMaxEdgeLoad() == 0);
    
    metrics.updateMaxEdgeLoad(5);
    assert(metrics.getMaxEdgeLoad() == 5);
    
    metrics.updateMaxEdgeLoad(3); // Should not decrease
    assert(metrics.getMaxEdgeLoad() == 5);
    
    metrics.updateMaxEdgeLoad(10);
    assert(metrics.getMaxEdgeLoad() == 10);
    
    std::cout << "✓ Max edge load tracked correctly: " 
              << metrics.getMaxEdgeLoad() << "\n";
}

void testTick() {
    std::cout << "\nTest: Tick Functionality\n";
    
    Metrics metrics;
    assert(metrics.getCurrentTick() == 0);
    assert(metrics.getThroughputPerTick().empty());
    assert(metrics.getEdgeLoadHistory().empty());
    
    metrics.tick();
    assert(metrics.getCurrentTick() == 1);
    assert(metrics.getThroughputPerTick().size() == 1);
    assert(metrics.getThroughputPerTick().back() == 0); // Initialized to 0
    assert(metrics.getEdgeLoadHistory().size() == 1);
    
    metrics.tick();
    assert(metrics.getCurrentTick() == 2);
    assert(metrics.getThroughputPerTick().size() == 2);
    assert(metrics.getThroughputPerTick().back() == 0);
    assert(metrics.getEdgeLoadHistory().size() == 2);
    
    std::cout << "✓ Tick increments counter and initializes per-tick data\n";
}

void testThroughputPerTick() {
    std::cout << "\nTest: Throughput Per Tick\n";
    
    Metrics metrics;
    Agent agent1(1, 0, 5);
    Agent agent2(2, 1, 6);
    
    metrics.tick(); // Tick 1
    metrics.recordArrival(agent1, 10);
    assert(metrics.getThroughputPerTick().back() == 1);
    
    metrics.tick(); // Tick 2
    metrics.recordArrival(agent2, 15);
    assert(metrics.getThroughputPerTick().back() == 1);
    
    metrics.tick(); // Tick 3, no arrivals
    assert(metrics.getThroughputPerTick().back() == 0);
    
    assert(metrics.getThroughputPerTick().size() == 3);
    assert(metrics.getThroughputPerTick()[0] == 1);
    assert(metrics.getThroughputPerTick()[1] == 1);
    assert(metrics.getThroughputPerTick()[2] == 0);
    
    std::cout << "✓ Throughput per tick recorded correctly\n";
}

void testSnapshotEdgeLoads() {
    std::cout << "\nTest: Snapshot Edge Loads\n";
    
    Metrics metrics;
    City city; // Dummy city for compilation
    
    metrics.tick();
    metrics.snapshotEdgeLoads(city);
    assert(metrics.getEdgeLoadHistory().size() == 1);
    // The actual content of edgeLoadHistory is populated by SimulationController
    // For now, we just check the structure is created.
    
    metrics.tick();
    metrics.snapshotEdgeLoads(city);
    assert(metrics.getEdgeLoadHistory().size() == 2);
    
    std::cout << "✓ snapshotEdgeLoads() creates history entries\n";
}

void testReset() {
    std::cout << "\nTest: Reset Functionality\n";
    
    Metrics metrics;
    Agent agent1(1, 0, 5);
    
    metrics.tick();
    metrics.recordArrival(agent1, 10);
    metrics.updateMaxEdgeLoad(5);
    metrics.snapshotEdgeLoads(City());
    
    assert(metrics.getCurrentTick() == 1);
    assert(metrics.totalThroughput() == 1);
    assert(metrics.getMaxEdgeLoad() == 5);
    assert(!metrics.getTripTimes().empty());
    assert(!metrics.getThroughputPerTick().empty());
    assert(!metrics.getEdgeLoadHistory().empty());
    
    metrics.reset();
    
    assert(metrics.getTripTimes().empty());
    assert(metrics.getThroughputPerTick().empty());
    assert(metrics.getEdgeLoadHistory().empty());
    assert(metrics.getMaxEdgeLoad() == 0);
    assert(metrics.getCurrentTick() == 0);
    assert(metrics.averageTripTime() == 0.0);
    assert(metrics.totalThroughput() == 0);
    
    std::cout << "✓ Metrics reset to initial state\n";
}

int main() {
    std::cout << "=== Metrics Test Suite ===\n\n";
    
    try {
        testInitialState();
        testRecordDeparture();
        testRecordArrival();
        testAverageTripTime();
        testTotalThroughput();
        testMaxEdgeLoad();
        testTick();
        testThroughputPerTick();
        testSnapshotEdgeLoads();
        testReset();
        
        std::cout << "\n=== All Metrics Tests Passed! ===\n";
        std::cout << "✓ Accurately tracks trip times\n";
        std::cout << "✓ Records throughput per tick\n";
        std::cout << "✓ Captures maximum edge load across simulation\n";
        std::cout << "✓ Calculations are correct\n";
        std::cout << "✓ Compiles without errors\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}

