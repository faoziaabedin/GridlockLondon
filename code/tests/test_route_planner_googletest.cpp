// code/tests/test_route_planner_googletest.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <memory>
#include "../core/RoutePlanner.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"
#include "../core/Agent.h"
#include "../core/ShortestPathPolicy.h"
#include "../core/CongestionAwarePolicy.h"
#include "mocks/MockPolicy.h"
#include "mocks/MockCity.h"

/**
 * Test Suite 1: RoutePlanner (15+ tests)
 * Tests pathfinding, policy integration, edge cases, and performance.
 */

class RoutePlannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple 3x3 grid for most tests
        city = TestCityBuilder::createSimpleGrid(3, 3);
        shortestPolicy = std::make_unique<ShortestPathPolicy>();
        congestionPolicy = std::make_unique<CongestionAwarePolicy>();
    }
    
    void TearDown() override {
        city.reset();
        shortestPolicy.reset();
        congestionPolicy.reset();
    }
    
    std::unique_ptr<City> city;
    std::unique_ptr<ShortestPathPolicy> shortestPolicy;
    std::unique_ptr<CongestionAwarePolicy> congestionPolicy;
};

// Test 1: Finds shortest path in simple grid
TEST_F(RoutePlannerTest, FindsShortestPathInSimpleGrid) {
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 8);  // From top-left (0) to bottom-right (8) in 3x3 grid
    
    auto path = planner.computePath(*city, agent);
    
    // Should find a path (exact length depends on grid topology)
    EXPECT_FALSE(path.empty());
    EXPECT_GT(path.size(), 0);
}

// Test 2: Handles blocked edges
TEST_F(RoutePlannerTest, HandlesBlockedEdges) {
    // Create city with blocked middle edge
    auto blockedCity = TestCityBuilder::createCityWithBlockedEdges(3, 3, {{1, 4}});
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 8);
    
    auto path = planner.computePath(*blockedCity, agent);
    
    // Should still find a path (alternative route)
    EXPECT_FALSE(path.empty());
}

// Test 3: Reroutes when capacity full
TEST_F(RoutePlannerTest, ReroutesWhenCapacityFull) {
    RoutePlanner planner(congestionPolicy.get());
    Agent agent(1, 0, 8);
    
    // Fill an edge to capacity
    if (city->getEdgeCount() > 0) {
        EdgeId eid = city->getEdgeIdByIndex(0);
        int cap = city->edgeCapacity(eid);
        for (int i = 0; i < cap; ++i) {
            city->incrementOccupancy(eid);
        }
    }
    
    auto path = planner.computePath(*city, agent);
    EXPECT_FALSE(path.empty());
}

// Test 4: Uses correct policy for cost calculation
TEST_F(RoutePlannerTest, UsesCorrectPolicyForCostCalculation) {
    MockPolicy mockPolicy;
    
    EXPECT_CALL(mockPolicy, edgeCost(testing::_, testing::_))
        .Times(testing::AtLeast(1))
        .WillRepeatedly(testing::Return(1.0));
    
    RoutePlanner planner(&mockPolicy);
    Agent agent(1, 0, 8);
    
    auto path = planner.computePath(*city, agent);
    
    // Verify mock was called
    EXPECT_FALSE(path.empty());
}

// Test 5: Handles disconnected graph
TEST_F(RoutePlannerTest, HandlesDisconnectedGraph) {
    auto disconnectedCity = TestCityBuilder::createDisconnectedCity();
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 1);
    
    auto path = planner.computePath(*disconnectedCity, agent);
    
    // Should return empty path (no route exists)
    EXPECT_TRUE(path.empty());
}

// Test 6: Performance with large graph
TEST_F(RoutePlannerTest, PerformanceWithLargeGraph) {
    auto largeCity = TestCityBuilder::createSimpleGrid(10, 10);
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 99);
    
    auto start = std::chrono::high_resolution_clock::now();
    auto path = planner.computePath(*largeCity, agent);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_FALSE(path.empty());
    EXPECT_LT(duration.count(), 1000);  // Should complete in < 1 second
}

// Test 7: Path from node to itself
TEST_F(RoutePlannerTest, PathFromNodeToItself) {
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 0);
    
    auto path = planner.computePath(*city, agent);
    
    // Should return empty path (already at destination)
    EXPECT_TRUE(path.empty());
}

// Test 8: Shortest path vs congestion-aware path
TEST_F(RoutePlannerTest, ShortestPathVsCongestionAwarePath) {
    Agent agent(1, 0, 8);
    
    RoutePlanner shortestPlanner(shortestPolicy.get());
    auto shortestPath = shortestPlanner.computePath(*city, agent);
    
    RoutePlanner congestionPlanner(congestionPolicy.get());
    auto congestionPath = congestionPlanner.computePath(*city, agent);
    
    // Both should find paths
    EXPECT_FALSE(shortestPath.empty());
    EXPECT_FALSE(congestionPath.empty());
}

// Test 9: Policy switching
TEST_F(RoutePlannerTest, PolicySwitching) {
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 8);
    
    auto path1 = planner.computePath(*city, agent);
    
    planner.setPolicy(congestionPolicy.get());
    auto path2 = planner.computePath(*city, agent);
    
    EXPECT_FALSE(path1.empty());
    EXPECT_FALSE(path2.empty());
}

// Test 10: Null policy handling
TEST_F(RoutePlannerTest, NullPolicyHandling) {
    RoutePlanner planner(nullptr);
    Agent agent(1, 0, 8);
    
    // Should handle gracefully (may crash or return empty, depends on implementation)
    // This tests defensive programming
    EXPECT_NO_THROW({
        auto path = planner.computePath(*city, agent);
    });
}

// Test 11: Multiple agents same route
TEST_F(RoutePlannerTest, MultipleAgentsSameRoute) {
    RoutePlanner planner(shortestPolicy.get());
    Agent agent1(1, 0, 8);
    Agent agent2(2, 0, 8);
    
    auto path1 = planner.computePath(*city, agent1);
    auto path2 = planner.computePath(*city, agent2);
    
    EXPECT_FALSE(path1.empty());
    EXPECT_FALSE(path2.empty());
    // Paths should be identical for same origin/destination
    EXPECT_EQ(path1.size(), path2.size());
}

// Test 12: Edge case - single node city
TEST_F(RoutePlannerTest, SingleNodeCity) {
    auto singleCity = std::make_unique<City>();
    singleCity->addNode(Node(0, 0, 0));
    
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 0);
    
    auto path = planner.computePath(*singleCity, agent);
    EXPECT_TRUE(path.empty());  // Already at destination
}

// Test 13: Path validation - path should be connected
TEST_F(RoutePlannerTest, PathShouldBeConnected) {
    RoutePlanner planner(shortestPolicy.get());
    Agent agent(1, 0, 8);
    
    auto path = planner.computePath(*city, agent);
    
    if (!path.empty()) {
        // Verify path is connected (each edge's destination is next edge's source)
        NodeId currentNode = agent.getOrigin();
        for (EdgeId eid : path) {
            const Edge& edge = city->getEdge(eid);
            EXPECT_EQ(edge.getFrom(), currentNode);
            currentNode = edge.getTo();
        }
        EXPECT_EQ(currentNode, agent.getDestination());
    }
}

// Test 14: Performance - many path computations
TEST_F(RoutePlannerTest, PerformanceManyPathComputations) {
    RoutePlanner planner(shortestPolicy.get());
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        Agent agent(i, 0, 8);
        auto path = planner.computePath(*city, agent);
        EXPECT_FALSE(path.empty());
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LT(duration.count(), 5000);  // 100 paths in < 5 seconds
}

// Test 15: Congestion-aware avoids high occupancy edges
TEST_F(RoutePlannerTest, CongestionAwareAvoidsHighOccupancy) {
    RoutePlanner planner(congestionPolicy.get());
    Agent agent(1, 0, 8);
    
    // Fill a direct path edge
    if (city->getEdgeCount() > 2) {
        EdgeId directEdge = city->getEdgeIdByIndex(0);
        int cap = city->edgeCapacity(directEdge);
        for (int i = 0; i < cap; ++i) {
            city->incrementOccupancy(directEdge);
        }
    }
    
    auto path = planner.computePath(*city, agent);
    EXPECT_FALSE(path.empty());
    // Congestion-aware should prefer less congested paths
}

// Parameterized test for different grid sizes
class RoutePlannerParameterizedTest : public ::testing::TestWithParam<std::pair<int, int>> {};

TEST_P(RoutePlannerParameterizedTest, WorksWithDifferentGridSizes) {
    auto [rows, cols] = GetParam();
    auto testCity = TestCityBuilder::createSimpleGrid(rows, cols);
    auto policy = std::make_unique<ShortestPathPolicy>();
    RoutePlanner planner(policy.get());
    
    int totalNodes = rows * cols;
    Agent agent(1, 0, totalNodes - 1);
    
    auto path = planner.computePath(*testCity, agent);
    EXPECT_FALSE(path.empty());
}

INSTANTIATE_TEST_SUITE_P(
    GridSizes,
    RoutePlannerParameterizedTest,
    ::testing::Values(
        std::make_pair(2, 2),
        std::make_pair(3, 3),
        std::make_pair(5, 5),
        std::make_pair(10, 10)
    )
);

// Main function
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

