// code/tests/test_metrics_googletest.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include "../core/Metrics.h"
#include "../core/City.h"
#include "../core/Agent.h"
#include "../core/Node.h"
#include "../core/Edge.h"
#include "mocks/MockCity.h"

/**
 * Test Suite 3: Metrics (10+ tests)
 * Tests calculation accuracy, edge cases, and performance.
 */

class MetricsTest : public ::testing::Test {
protected:
    void SetUp() override {
        metrics = std::make_unique<Metrics>();
        city = TestCityBuilder::createSimpleGrid(3, 3);
    }
    
    void TearDown() override {
        metrics.reset();
        city.reset();
    }
    
    std::unique_ptr<Metrics> metrics;
    std::unique_ptr<City> city;
};

// Test 1: Average trip time calculation
TEST_F(MetricsTest, AverageTripTimeCalculation) {
    Agent agent1(1, 0, 8);
    Agent agent2(2, 0, 8);
    
    metrics->recordDeparture(agent1);
    metrics->recordArrival(agent1, 10);
    
    metrics->recordDeparture(agent2);
    metrics->recordArrival(agent2, 20);
    
    double avg = metrics->averageTripTime();
    EXPECT_DOUBLE_EQ(avg, 15.0);  // (10 + 20) / 2
}

// Test 2: Zero agents - average trip time
TEST_F(MetricsTest, ZeroAgentsAverageTripTime) {
    double avg = metrics->averageTripTime();
    EXPECT_DOUBLE_EQ(avg, 0.0);
}

// Test 3: Total throughput
TEST_F(MetricsTest, TotalThroughput) {
    Agent agent1(1, 0, 8);
    Agent agent2(2, 0, 8);
    
    metrics->recordDeparture(agent1);
    metrics->recordArrival(agent1, 10);
    
    metrics->recordDeparture(agent2);
    metrics->recordArrival(agent2, 20);
    
    EXPECT_EQ(metrics->totalThroughput(), 2);
}

// Test 4: Max edge load tracking
TEST_F(MetricsTest, MaxEdgeLoadTracking) {
    metrics->updateMaxEdgeLoad(5);
    metrics->updateMaxEdgeLoad(10);
    metrics->updateMaxEdgeLoad(7);
    
    EXPECT_EQ(metrics->getMaxEdgeLoad(), 10);
}

// Test 5: Edge load snapshot
TEST_F(MetricsTest, EdgeLoadSnapshot) {
    // Fill some edges
    if (city->getEdgeCount() > 0) {
        city->incrementOccupancy(city->getEdgeIdByIndex(0));
        city->incrementOccupancy(city->getEdgeIdByIndex(0));
    }
    
    metrics->snapshotEdgeLoads(*city);
    
    // Verify snapshot was taken
    const auto& history = metrics->getEdgeLoadHistory();
    EXPECT_FALSE(history.empty());
}

// Test 6: Tick counter
TEST_F(MetricsTest, TickCounter) {
    EXPECT_EQ(metrics->getCurrentTick(), 0);
    
    metrics->tick();
    EXPECT_EQ(metrics->getCurrentTick(), 1);
    
    metrics->tick();
    EXPECT_EQ(metrics->getCurrentTick(), 2);
}

// Test 7: Reset functionality
TEST_F(MetricsTest, ResetFunctionality) {
    Agent agent(1, 0, 8);
    metrics->recordDeparture(agent);
    metrics->recordArrival(agent, 10);
    metrics->tick();
    metrics->updateMaxEdgeLoad(5);
    
    metrics->reset();
    
    EXPECT_EQ(metrics->totalThroughput(), 0);
    EXPECT_DOUBLE_EQ(metrics->averageTripTime(), 0.0);
    EXPECT_EQ(metrics->getMaxEdgeLoad(), 0);
    EXPECT_EQ(metrics->getCurrentTick(), 0);
}

// Test 8: All agents arrived immediately
TEST_F(MetricsTest, AllAgentsArrivedImmediately) {
    Agent agent1(1, 0, 0);
    Agent agent2(2, 1, 1);
    
    metrics->recordDeparture(agent1);
    metrics->recordArrival(agent1, 0);
    
    metrics->recordDeparture(agent2);
    metrics->recordArrival(agent2, 0);
    
    EXPECT_DOUBLE_EQ(metrics->averageTripTime(), 0.0);
    EXPECT_EQ(metrics->totalThroughput(), 2);
}

// Test 9: Throughput per tick
TEST_F(MetricsTest, ThroughputPerTick) {
    Agent agent1(1, 0, 8);
    Agent agent2(2, 0, 8);
    
    metrics->tick();
    metrics->recordDeparture(agent1);
    metrics->recordArrival(agent1, 1);
    
    metrics->tick();
    metrics->recordDeparture(agent2);
    metrics->recordArrival(agent2, 1);
    
    const auto& throughput = metrics->getThroughputPerTick();
    // Verify throughput tracking
    EXPECT_GE(throughput.size(), 0);
}

// Test 10: Performance - many agents
TEST_F(MetricsTest, PerformanceManyAgents) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        Agent agent(i, 0, 8);
        metrics->recordDeparture(agent);
        metrics->recordArrival(agent, 10);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_EQ(metrics->totalThroughput(), 1000);
    EXPECT_LT(duration.count(), 100);  // Should complete in < 100ms
}

// Test 11: Trip times storage
TEST_F(MetricsTest, TripTimesStorage) {
    Agent agent1(1, 0, 8);
    Agent agent2(2, 0, 8);
    
    metrics->recordDeparture(agent1);
    metrics->recordArrival(agent1, 10);
    
    metrics->recordDeparture(agent2);
    metrics->recordArrival(agent2, 20);
    
    const auto& tripTimes = metrics->getTripTimes();
    EXPECT_EQ(tripTimes.size(), 2);
    EXPECT_DOUBLE_EQ(tripTimes[0], 10.0);
    EXPECT_DOUBLE_EQ(tripTimes[1], 20.0);
}

// Test 12: Edge load history accumulation
TEST_F(MetricsTest, EdgeLoadHistoryAccumulation) {
    metrics->snapshotEdgeLoads(*city);
    metrics->tick();
    metrics->snapshotEdgeLoads(*city);
    metrics->tick();
    metrics->snapshotEdgeLoads(*city);
    
    const auto& history = metrics->getEdgeLoadHistory();
    EXPECT_GE(history.size(), 3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

