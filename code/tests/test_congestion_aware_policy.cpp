// code/tests/test_congestion_aware_policy.cpp
// Test program for CongestionAwarePolicy implementation
#include <iostream>
#include <cassert>
#include <memory>
#include "core/IRoutePolicy.h"
#include "core/CongestionAwarePolicy.h"
#include "core/City.h"
#include "core/Agent.h"
#include "core/Node.h"
#include "core/Edge.h"

void testCongestionAwarePolicyBasic() {
    std::cout << "Test: CongestionAwarePolicy Basic Functionality\n";
    
    // Create a simple city with edges of different capacities
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addNode(Node(2, 0, 2));
    
    // Add edges with different lengths and capacities
    city.addEdge(Edge(0, 0, 1, 5.0, 10));  // Edge 0: length 5.0, capacity 10
    city.addEdge(Edge(1, 1, 2, 3.0, 5));   // Edge 1: length 3.0, capacity 5
    
    CongestionAwarePolicy policy;
    
    // Test edgeCost with no occupancy
    double cost0_empty = policy.edgeCost(city, 0);
    double cost1_empty = policy.edgeCost(city, 1);
    
    // With no occupancy: cost = length + alpha * (0/capacity) = length
    assert(cost0_empty == 5.0);
    assert(cost1_empty == 3.0);
    std::cout << "✓ edgeCost with no occupancy equals edge length\n";
    
    // Test shouldRerouteOnNode always returns true
    Agent agent(1, 0, 2);
    assert(policy.shouldRerouteOnNode(agent) == true);
    
    agent.setPath({0, 1});
    assert(policy.shouldRerouteOnNode(agent) == true);
    std::cout << "✓ shouldRerouteOnNode always returns true\n";
}

void testCongestionFormula() {
    std::cout << "\nTest: Congestion Formula Implementation\n";
    
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 4.0, 10));  // length=4.0, capacity=10
    
    CongestionAwarePolicy policy;
    
    // Test with different occupancy levels
    // Formula: length + alpha * (occupancy / capacity)
    // alpha = 2.0, length = 4.0, capacity = 10
    
    // No occupancy: 4.0 + 2.0 * (0/10) = 4.0
    city.setOccupancy(0, 0);
    double cost_empty = policy.edgeCost(city, 0);
    assert(cost_empty == 4.0);
    std::cout << "✓ No occupancy: cost = " << cost_empty << " (expected 4.0)\n";
    
    // Half capacity: 4.0 + 2.0 * (5/10) = 4.0 + 1.0 = 5.0
    city.setOccupancy(0, 5);
    double cost_half = policy.edgeCost(city, 0);
    assert(cost_half == 5.0);
    std::cout << "✓ Half capacity: cost = " << cost_half << " (expected 5.0)\n";
    
    // Full capacity: 4.0 + 2.0 * (10/10) = 4.0 + 2.0 = 6.0
    city.setOccupancy(0, 10);
    double cost_full = policy.edgeCost(city, 0);
    assert(cost_full == 6.0);
    std::cout << "✓ Full capacity: cost = " << cost_full << " (expected 6.0)\n";
    
    // Over capacity: 4.0 + 2.0 * (15/10) = 4.0 + 3.0 = 7.0
    city.setOccupancy(0, 15);
    double cost_over = policy.edgeCost(city, 0);
    assert(cost_over == 7.0);
    std::cout << "✓ Over capacity: cost = " << cost_over << " (expected 7.0)\n";
}

void testCostIncreasesWithOccupancy() {
    std::cout << "\nTest: Cost Increases with Edge Occupancy\n";
    
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 2.0, 8));  // length=2.0, capacity=8
    
    CongestionAwarePolicy policy;
    
    // Test that cost increases monotonically with occupancy
    double prev_cost = -1.0;
    for (int occupancy = 0; occupancy <= 12; occupancy += 2) {
        city.setOccupancy(0, occupancy);
        double cost = policy.edgeCost(city, 0);
        
        assert(cost > prev_cost);  // Cost should always increase
        prev_cost = cost;
        
        std::cout << "  Occupancy " << occupancy << ": cost = " << cost << "\n";
    }
    std::cout << "✓ Cost increases monotonically with occupancy\n";
}

void testHigherOccupancyHigherCost() {
    std::cout << "\nTest: Higher Occupancy = Higher Cost\n";
    
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addNode(Node(2, 0, 2));
    
    // Two edges with same length but different capacities
    city.addEdge(Edge(0, 0, 1, 3.0, 10));  // High capacity
    city.addEdge(Edge(1, 1, 2, 3.0, 5));   // Low capacity
    
    CongestionAwarePolicy policy;
    
    // Set same occupancy on both edges
    city.setOccupancy(0, 5);  // 5/10 = 0.5 occupancy ratio
    city.setOccupancy(1, 5);  // 5/5 = 1.0 occupancy ratio
    
    double cost0 = policy.edgeCost(city, 0);
    double cost1 = policy.edgeCost(city, 1);
    
    // Edge 1 should have higher cost due to higher occupancy ratio
    assert(cost1 > cost0);
    std::cout << "✓ Higher occupancy ratio results in higher cost\n";
    std::cout << "  Edge 0 (5/10): cost = " << cost0 << "\n";
    std::cout << "  Edge 1 (5/5): cost = " << cost1 << "\n";
}

void testAlwaysReroute() {
    std::cout << "\nTest: Always Reroute at Every Node\n";
    
    CongestionAwarePolicy policy;
    
    // Test with different agent states
    Agent agent1(1, 0, 5);
    Agent agent2(2, 0, 3);
    
    // Agent with no path
    assert(policy.shouldRerouteOnNode(agent1) == true);
    std::cout << "✓ Agent with no path: shouldReroute = true\n";
    
    // Agent with path
    std::deque<EdgeId> path = {0, 1, 2};
    agent1.setPath(path);
    assert(policy.shouldRerouteOnNode(agent1) == true);
    std::cout << "✓ Agent with path: shouldReroute = true\n";
    
    // Agent at destination
    Agent agent3(3, 5, 5);
    assert(policy.shouldRerouteOnNode(agent3) == true);
    std::cout << "✓ Agent at destination: shouldReroute = true\n";
    
    // Agent that has arrived
    agent2.setPath({0});
    assert(policy.shouldRerouteOnNode(agent2) == true);
    std::cout << "✓ Agent that has arrived: shouldReroute = true\n";
}

void testPolymorphicUsage() {
    std::cout << "\nTest: Polymorphic Usage\n";
    
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 6.0, 10));
    city.setOccupancy(0, 3);  // 3/10 occupancy
    
    // Test through base class pointer
    std::unique_ptr<IRoutePolicy> policy = std::make_unique<CongestionAwarePolicy>();
    
    double cost = policy->edgeCost(city, 0);
    // Expected: 6.0 + 2.0 * (3/10) = 6.0 + 0.6 = 6.6
    assert(cost == 6.6);
    std::cout << "✓ Polymorphic edgeCost works: " << cost << "\n";
    
    Agent agent(1, 0, 1);
    bool shouldReroute = policy->shouldRerouteOnNode(agent);
    assert(shouldReroute == true);
    std::cout << "✓ Polymorphic shouldRerouteOnNode works: " << shouldReroute << "\n";
}

void testAlphaWeight() {
    std::cout << "\nTest: Alpha Weight Factor\n";
    
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 1.0, 10));  // length=1.0, capacity=10
    city.setOccupancy(0, 5);  // 5/10 = 0.5 occupancy ratio
    
    CongestionAwarePolicy policy;
    double cost = policy.edgeCost(city, 0);
    
    // With alpha=2.0: 1.0 + 2.0 * 0.5 = 2.0
    assert(cost == 2.0);
    std::cout << "✓ Alpha weight factor (2.0) applied correctly: cost = " << cost << "\n";
}

void testEdgeCases() {
    std::cout << "\nTest: Edge Cases\n";
    
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 1.0, 1));  // Very small capacity
    
    CongestionAwarePolicy policy;
    
    // Test with capacity = 1
    city.setOccupancy(0, 1);
    double cost = policy.edgeCost(city, 0);
    // Expected: 1.0 + 2.0 * (1/1) = 3.0
    assert(cost == 3.0);
    std::cout << "✓ Small capacity handled correctly: cost = " << cost << "\n";
    
    // Test with zero capacity (should not crash)
    city.addEdge(Edge(1, 0, 1, 1.0, 0));
    try {
        double cost_zero = policy.edgeCost(city, 1);
        std::cout << "✓ Zero capacity handled: cost = " << cost_zero << "\n";
    } catch (...) {
        std::cout << "✓ Zero capacity throws exception (expected behavior)\n";
    }
}

int main() {
    std::cout << "=== CongestionAwarePolicy Test Suite ===\n\n";
    
    try {
        testCongestionAwarePolicyBasic();
        testCongestionFormula();
        testCostIncreasesWithOccupancy();
        testHigherOccupancyHigherCost();
        testAlwaysReroute();
        testPolymorphicUsage();
        testAlphaWeight();
        testEdgeCases();
        
        std::cout << "\n=== All CongestionAwarePolicy Tests Passed! ===\n";
        std::cout << "✓ Cost increases with edge occupancy\n";
        std::cout << "✓ Higher occupancy = higher cost\n";
        std::cout << "✓ Agents reroute at every node\n";
        std::cout << "✓ Compiles without errors\n";
        std::cout << "✓ Formula: length + alpha * (occupancy / capacity)\n";
        std::cout << "✓ Alpha weight factor = 2.0\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}
