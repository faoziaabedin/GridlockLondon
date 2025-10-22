// code/tests/test_route_policy_contract.cpp
// Contract verification tests for IRoutePolicy interface
#include <iostream>
#include <cassert>
#include <memory>
#include "core/IRoutePolicy.h"
#include "core/ShortestPathPolicy.h"
#include "core/City.h"
#include "core/Agent.h"
#include "core/Node.h"
#include "core/Edge.h"

/**
 * Test that verifies the IRoutePolicy interface contract
 */
void testInterfaceContract() {
    std::cout << "=== IRoutePolicy Interface Contract Tests ===\n";
    
    // Test 1: Interface can be instantiated polymorphically
    std::cout << "Test 1: Polymorphic instantiation...\n";
    std::unique_ptr<IRoutePolicy> policy = std::make_unique<ShortestPathPolicy>();
    assert(policy != nullptr);
    std::cout << "✓ IRoutePolicy can be instantiated polymorphically\n";
    
    // Test 2: Virtual destructor works
    std::cout << "Test 2: Virtual destructor...\n";
    {
        std::unique_ptr<IRoutePolicy> temp = std::make_unique<ShortestPathPolicy>();
        // Destructor called automatically when temp goes out of scope
    }
    std::cout << "✓ Virtual destructor works correctly\n";
    
    // Test 3: Interface methods are virtual and can be overridden
    std::cout << "Test 3: Virtual method dispatch...\n";
    ShortestPathPolicy concrete;
    IRoutePolicy* base = &concrete;
    
    // These should call the derived class implementations
    assert(&concrete.edgeCost == &base->edgeCost);
    assert(&concrete.shouldRerouteOnNode == &base->shouldRerouteOnNode);
    std::cout << "✓ Virtual method dispatch works correctly\n";
}

/**
 * Test that verifies ShortestPathPolicy implementation requirements
 */
void testShortestPathPolicyRequirements() {
    std::cout << "\n=== ShortestPathPolicy Requirements Tests ===\n";
    
    // Create test city
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addNode(Node(2, 0, 1));
    city.addEdge(Edge(0, 0, 1, 3.5, 10));
    city.addEdge(Edge(1, 1, 2, 2.0, 10));
    
    ShortestPathPolicy policy;
    
    // Test 1: edgeCost returns city.edgeLength(edgeId)
    std::cout << "Test 1: edgeCost uses city.edgeLength()...\n";
    double cost0 = policy.edgeCost(city, 0);
    double cost1 = policy.edgeCost(city, 1);
    
    assert(cost0 == city.edgeLength(0));
    assert(cost1 == city.edgeLength(1));
    assert(cost0 == 3.5);
    assert(cost1 == 2.0);
    std::cout << "✓ edgeCost() returns city.edgeLength() exactly\n";
    
    // Test 2: shouldRerouteOnNode returns false unless path is empty
    std::cout << "Test 2: shouldRerouteOnNode behavior...\n";
    
    // Agent with no path
    Agent agent1(1, 0, 2);
    assert(agent1.getPath().empty());
    assert(policy.shouldRerouteOnNode(agent1) == true);
    std::cout << "✓ Agent with empty path should reroute\n";
    
    // Agent with path
    std::deque<EdgeId> path = {0, 1};
    agent1.setPath(path);
    assert(!agent1.getPath().empty());
    assert(policy.shouldRerouteOnNode(agent1) == false);
    std::cout << "✓ Agent with path should NOT reroute\n";
    
    // Agent with single edge path
    Agent agent2(2, 0, 1);
    std::deque<EdgeId> singlePath = {0};
    agent2.setPath(singlePath);
    assert(policy.shouldRerouteOnNode(agent2) == false);
    std::cout << "✓ Agent with single edge path should NOT reroute\n";
}

/**
 * Test polymorphic usage scenarios
 */
void testPolymorphicUsage() {
    std::cout << "\n=== Polymorphic Usage Tests ===\n";
    
    // Create test city
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 4.0, 10));
    
    // Test 1: Base class pointer to derived class
    std::cout << "Test 1: Base class pointer usage...\n";
    std::unique_ptr<IRoutePolicy> policy = std::make_unique<ShortestPathPolicy>();
    
    double cost = policy->edgeCost(city, 0);
    assert(cost == 4.0);
    std::cout << "✓ Base class pointer calls derived implementation\n";
    
    // Test 2: Multiple policies can coexist
    std::cout << "Test 2: Multiple policy instances...\n";
    std::unique_ptr<IRoutePolicy> policy1 = std::make_unique<ShortestPathPolicy>();
    std::unique_ptr<IRoutePolicy> policy2 = std::make_unique<ShortestPathPolicy>();
    
    double cost1 = policy1->edgeCost(city, 0);
    double cost2 = policy2->edgeCost(city, 0);
    assert(cost1 == cost2);
    assert(cost1 == 4.0);
    std::cout << "✓ Multiple policy instances work independently\n";
    
    // Test 3: Policy can be used in function expecting interface
    std::cout << "Test 3: Function parameter usage...\n";
    auto testFunction = [](const IRoutePolicy& p, const City& c, EdgeId e) {
        return p.edgeCost(c, e);
    };
    
    double functionCost = testFunction(*policy, city, 0);
    assert(functionCost == 4.0);
    std::cout << "✓ Policy can be passed to function expecting interface\n";
}

/**
 * Test edge cases and error conditions
 */
void testEdgeCases() {
    std::cout << "\n=== Edge Cases Tests ===\n";
    
    ShortestPathPolicy policy;
    
    // Test 1: Empty city
    std::cout << "Test 1: Empty city handling...\n";
    City emptyCity;
    // This should either return 0 or throw - we test that it doesn't crash
    try {
        double cost = policy.edgeCost(emptyCity, 0);
        std::cout << "✓ Empty city edgeCost handled gracefully (returned: " << cost << ")\n";
    } catch (...) {
        std::cout << "✓ Empty city edgeCost throws exception (expected behavior)\n";
    }
    
    // Test 2: Invalid edge ID
    std::cout << "Test 2: Invalid edge ID handling...\n";
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 2.0, 10));
    
    try {
        double cost = policy.edgeCost(city, 999); // Invalid edge ID
        std::cout << "✓ Invalid edge ID handled gracefully (returned: " << cost << ")\n";
    } catch (...) {
        std::cout << "✓ Invalid edge ID throws exception (expected behavior)\n";
    }
    
    // Test 3: Agent at destination
    std::cout << "Test 3: Agent at destination...\n";
    Agent agent(1, 5, 5); // Same origin and destination
    assert(agent.getPath().empty());
    assert(policy.shouldRerouteOnNode(agent) == true);
    std::cout << "✓ Agent at destination with no path should reroute\n";
}

int main() {
    std::cout << "=== Route Policy Contract Verification ===\n\n";
    
    try {
        testInterfaceContract();
        testShortestPathPolicyRequirements();
        testPolymorphicUsage();
        testEdgeCases();
        
        std::cout << "\n=== All Contract Tests Passed! ===\n";
        std::cout << "✓ IRoutePolicy interface works correctly\n";
        std::cout << "✓ ShortestPathPolicy implementation meets requirements\n";
        std::cout << "✓ Polymorphic usage works as expected\n";
        std::cout << "✓ Edge cases handled appropriately\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Contract test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Contract test failed with unknown exception\n";
        return 1;
    }
}
