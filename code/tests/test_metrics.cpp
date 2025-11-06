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
    
    // Initially zero
    assert(metrics.totalThroughput() == 0);
    
    // Record arrivals
    Agent agent1(1, 0, 5);
    Agent agent2(2, 1, 6);
    Agent agent3(3, 2, 7);
    
    metrics.recordArrival(agent1, 10);
    assert(metrics.totalThroughput() == 1);
    
    metrics.recordArrival(agent2, 15);
    assert(metrics.totalThroughput() == 2);
    
    metrics.recordArrival(agent3, 20);
    assert(metrics.totalThroughput() == 3);
    
    std::cout << "✓ Total throughput tracked correctly: " 
              << metrics.totalThroughput() << "\n";
}

void testMaxEdgeLoad() {
    std::cout << "\nTest: Max Edge Load\n";
    
    Metrics metrics;
    
    // Initially zero
    assert(metrics.getMaxEdgeLoad() == 0);
    
    // Update with increasing loads
    metrics.updateMaxEdgeLoad(5);
    assert(metrics.getMaxEdgeLoad() == 5);
    
    metrics.updateMaxEdgeLoad(3);  // Should not update (lower)
    assert(metrics.getMaxEdgeLoad() == 5);
    
    metrics.updateMaxEdgeLoad(10);  // Should update (higher)
    assert(metrics.getMaxEdgeLoad() == 10);
    
    metrics.updateMaxEdgeLoad(8);  // Should not update (lower)
    assert(metrics.getMaxEdgeLoad() == 10);
    
    metrics.updateMaxEdgeLoad(15);  // Should update (higher)
    assert(metrics.getMaxEdgeLoad() == 15);
    
    std::cout << "✓ Max edge load tracked correctly: " 
              << metrics.getMaxEdgeLoad() << "\n";
}

void testTick() {
    std::cout << "\nTest: Tick Functionality\n";
    
    Metrics metrics;
    
    // Initially at tick 0
    assert(metrics.getCurrentTick() == 0);
    
    // First tick
    metrics.tick();
    assert(metrics.getCurrentTick() == 1);
    assert(metrics.getThroughputPerTick().size() == 1);
    assert(metrics.getThroughputPerTick()[0] == 0);
    assert(metrics.getEdgeLoadHistory().size() == 1);
    
    // Second tick
    metrics.tick();
    assert(metrics.getCurrentTick() == 2);
    assert(metrics.getThroughputPerTick().size() == 2);
    assert(metrics.getEdgeLoadHistory().size() == 2);
    
    // Multiple ticks
    for (int i = 0; i < 10; ++i) {
        metrics.tick();
    }
    assert(metrics.getCurrentTick() == 12);
    assert(metrics.getThroughputPerTick().size() == 12);
    assert(metrics.getEdgeLoadHistory().size() == 12);
    
    std::cout << "✓ Tick counter increments correctly\n";
    std::cout << "✓ Throughput per tick initialized for each tick\n";
    std::cout << "✓ Edge load history initialized for each tick\n";
}

void testThroughputPerTick() {
    std::cout << "\nTest: Throughput Per Tick\n";
    
    Metrics metrics;
    
    // Start ticking
    metrics.tick();  // Tick 1
    metrics.tick();  // Tick 2
    
    // Record arrivals during tick 2
    Agent agent1(1, 0, 5);
    Agent agent2(2, 1, 6);
    
    metrics.recordArrival(agent1, 10);
    metrics.recordArrival(agent2, 15);
    
    // Throughput for current tick should be updated
    assert(metrics.getThroughputPerTick().size() >= 2);
    assert(metrics.getThroughputPerTick().back() >= 2);
    
    // Next tick
    metrics.tick();  // Tick 3
    
    // Record arrival during tick 3
    Agent agent3(3, 2, 7);
    metrics.recordArrival(agent3, 20);
    
    assert(metrics.getThroughputPerTick().size() >= 3);
    
    std::cout << "✓ Throughput per tick tracked correctly\n";
}

void testSnapshotEdgeLoads() {
    std::cout << "\nTest: Snapshot Edge Loads\n";
    
    Metrics metrics;
    City city;
    
    // Create a simple city for testing
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 1.0, 10));
    
    // Should not crash
    metrics.snapshotEdgeLoads(city);
    
    // Should initialize edge load history if empty
    metrics.tick();
    metrics.snapshotEdgeLoads(city);
    
    std::cout << "✓ snapshotEdgeLoads() executes without error\n";
}

void testReset() {
    std::cout << "\nTest: Reset Functionality\n";
    
    Metrics metrics;
    
    // Populate with data
    Agent agent1(1, 0, 5);
    Agent agent2(2, 1, 6);
    
    metrics.tick();
    metrics.recordArrival(agent1, 10);
    metrics.updateMaxEdgeLoad(5);
    metrics.tick();
    metrics.recordArrival(agent2, 15);
    metrics.updateMaxEdgeLoad(10);
    
    // Verify data exists
    assert(metrics.getTripTimes().size() == 2);
    assert(metrics.getMaxEdgeLoad() == 10);
    assert(metrics.getCurrentTick() == 2);
    
    // Reset
    metrics.reset();
    
    // Verify everything is cleared
    assert(metrics.getTripTimes().empty());
    assert(metrics.getThroughputPerTick().empty());
    assert(metrics.getEdgeLoadHistory().empty());
    assert(metrics.getMaxEdgeLoad() == 0);
    assert(metrics.getCurrentTick() == 0);
    assert(metrics.averageTripTime() == 0.0);
    assert(metrics.totalThroughput() == 0);
    
    std::cout << "✓ Reset clears all data correctly\n";
}

void testCalculationsAccuracy() {
    std::cout << "\nTest: Calculation Accuracy\n";
    
    Metrics metrics;
    
    // Record multiple trips with known values
    Agent agents[5] = {
        Agent(1, 0, 5),
        Agent(2, 1, 6),
        Agent(3, 2, 7),
        Agent(4, 3, 8),
        Agent(5, 4, 9)
    };
    
    int times[] = {10, 20, 30, 40, 50};
    double expectedAvg = (10 + 20 + 30 + 40 + 50) / 5.0;  // 30.0
    
    for (int i = 0; i < 5; ++i) {
        metrics.recordArrival(agents[i], times[i]);
    }
    
    // Verify average is correct
    double calculatedAvg = metrics.averageTripTime();
    assert(std::abs(calculatedAvg - expectedAvg) < 0.001);
    
    // Verify throughput is correct
    assert(metrics.totalThroughput() == 5);
    
    std::cout << "✓ Calculations are accurate\n";
    std::cout << "  Expected average: " << expectedAvg << "\n";
    std::cout << "  Calculated average: " << calculatedAvg << "\n";
}

void testEdgeLoadHistory() {
    std::cout << "\nTest: Edge Load History\n";
    
    Metrics metrics;
    
    // Initially empty
    assert(metrics.getEdgeLoadHistory().empty());
    
    // After tick, should have entry
    metrics.tick();
    assert(metrics.getEdgeLoadHistory().size() == 1);
    
    // Multiple ticks
    metrics.tick();
    metrics.tick();
    assert(metrics.getEdgeLoadHistory().size() == 3);
    
    // After reset, should be empty
    metrics.reset();
    assert(metrics.getEdgeLoadHistory().empty());
    
    std::cout << "✓ Edge load history structure maintained correctly\n";
}

void testMultipleTicksWithArrivals() {
    std::cout << "\nTest: Multiple Ticks with Arrivals\n";
    
    Metrics metrics;
    
    // Simulate multiple ticks with arrivals
    for (int tick = 0; tick < 5; ++tick) {
        metrics.tick();
        
        // Some agents arrive this tick
        for (int agent = 0; agent < (tick + 1); ++agent) {
            Agent a(agent, 0, 5);
            metrics.recordArrival(a, 10 + tick);
        }
    }
    
    // Verify counts
    assert(metrics.getCurrentTick() == 5);
    assert(metrics.getTripTimes().size() == 15);  // 1+2+3+4+5 = 15
    assert(metrics.totalThroughput() == 15);
    assert(metrics.getThroughputPerTick().size() == 5);
    
    std::cout << "✓ Multiple ticks with arrivals tracked correctly\n";
    std::cout << "  Total arrivals: " << metrics.totalThroughput() << "\n";
    std::cout << "  Ticks: " << metrics.getCurrentTick() << "\n";
}

void testMaxEdgeLoadTracking() {
    std::cout << "\nTest: Max Edge Load Tracking Across Simulation\n";
    
    Metrics metrics;
    
    // Simulate increasing edge loads
    int loads[] = {1, 3, 5, 2, 8, 4, 12, 6, 15, 10};
    int expectedMax = 15;
    
    for (int load : loads) {
        metrics.updateMaxEdgeLoad(load);
    }
    
    assert(metrics.getMaxEdgeLoad() == expectedMax);
    
    // Reset and verify
    metrics.reset();
    assert(metrics.getMaxEdgeLoad() == 0);
    
    // Track again
    for (int load : loads) {
        metrics.updateMaxEdgeLoad(load);
    }
    assert(metrics.getMaxEdgeLoad() == expectedMax);
    
    std::cout << "✓ Max edge load tracked across entire simulation: " 
              << metrics.getMaxEdgeLoad() << "\n";
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
        testCalculationsAccuracy();
        testEdgeLoadHistory();
        testMultipleTicksWithArrivals();
        testMaxEdgeLoadTracking();
        
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

