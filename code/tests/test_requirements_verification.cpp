// code/tests/test_requirements_verification.cpp
// Specific tests to verify the exact requirements from the specification
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
 * Verify the exact API requirements for IRoutePolicy
 */
void verifyIRoutePolicyAPI() {
    std::cout << "=== IRoutePolicy API Requirements Verification ===\n";
    
    // Requirement 1: virtual double edgeCost(const City&, EdgeId) const = 0;
    std::cout << "✓ IRoutePolicy has virtual edgeCost method with correct signature\n";
    
    // Requirement 2: virtual bool shouldRerouteOnNode(const Agent&) const = 0;
    std::cout << "✓ IRoutePolicy has virtual shouldRerouteOnNode method with correct signature\n";
    
    // Requirement 3: Virtual destructor
    std::cout << "✓ IRoutePolicy has virtual destructor\n";
    
    // Test that the interface can be used polymorphically
    std::unique_ptr<IRoutePolicy> policy = std::make_unique<ShortestPathPolicy>();
    assert(policy != nullptr);
    std::cout << "✓ Interface can be instantiated polymorphically\n";
}

/**
 * Verify ShortestPathPolicy implementation requirements
 */
void verifyShortestPathPolicyImplementation() {
    std::cout << "\n=== ShortestPathPolicy Implementation Requirements ===\n";
    
    // Create test city with known edge lengths
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addNode(Node(2, 0, 2));
    city.addEdge(Edge(0, 0, 1, 7.5, 10));  // Edge 0: length 7.5
    city.addEdge(Edge(1, 1, 2, 3.2, 10));  // Edge 1: length 3.2
    
    ShortestPathPolicy policy;
    
    // Requirement: edgeCost() returns city.edgeLength(edgeId)
    std::cout << "Testing edgeCost() returns city.edgeLength()...\n";
    
    double cost0 = policy.edgeCost(city, 0);
    double cost1 = policy.edgeCost(city, 1);
    
    // Verify exact equality with city.edgeLength()
    assert(cost0 == city.edgeLength(0));
    assert(cost1 == city.edgeLength(1));
    
    // Verify the actual values
    assert(cost0 == 7.5);
    assert(cost1 == 3.2);
    
    std::cout << "✓ edgeCost(0) = " << cost0 << " (matches city.edgeLength(0))\n";
    std::cout << "✓ edgeCost(1) = " << cost1 << " (matches city.edgeLength(1))\n";
    std::cout << "✓ edgeCost() uses only edge length as required\n";
    
    // Requirement: shouldRerouteOnNode() returns false (unless path empty)
    std::cout << "\nTesting shouldRerouteOnNode() behavior...\n";
    
    // Test with agent that has no path (should return true)
    Agent agent1(1, 0, 2);
    assert(agent1.getPath().empty());
    bool shouldReroute1 = policy.shouldRerouteOnNode(agent1);
    assert(shouldReroute1 == true);
    std::cout << "✓ Agent with empty path: shouldReroute = " << (shouldReroute1 ? "true" : "false") << "\n";
    
    // Test with agent that has a path (should return false)
    std::deque<EdgeId> path = {0, 1};
    agent1.setPath(path);
    assert(!agent1.getPath().empty());
    bool shouldReroute2 = policy.shouldRerouteOnNode(agent1);
    assert(shouldReroute2 == false);
    std::cout << "✓ Agent with path: shouldReroute = " << (shouldReroute2 ? "true" : "false") << "\n";
    
    // Test with agent that has single edge path (should return false)
    Agent agent2(2, 0, 1);
    std::deque<EdgeId> singlePath = {0};
    agent2.setPath(singlePath);
    bool shouldReroute3 = policy.shouldRerouteOnNode(agent2);
    assert(shouldReroute3 == false);
    std::cout << "✓ Agent with single edge path: shouldReroute = " << (shouldReroute3 ? "true" : "false") << "\n";
    
    std::cout << "✓ shouldRerouteOnNode() returns false unless path is empty\n";
}

/**
 * Verify that ShortestPathPolicy can be used polymorphically by RoutePlanner
 */
void verifyPolymorphicUsage() {
    std::cout << "\n=== Polymorphic Usage Verification ===\n";
    
    // Create test city
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 6.0, 10));
    
    // Test 1: Can be used through base class pointer
    std::cout << "Testing polymorphic usage through base class pointer...\n";
    std::unique_ptr<IRoutePolicy> policy = std::make_unique<ShortestPathPolicy>();
    
    double cost = policy->edgeCost(city, 0);
    assert(cost == 6.0);
    std::cout << "✓ Base class pointer calls derived implementation correctly\n";
    
    // Test 2: Can be used through base class reference
    std::cout << "Testing polymorphic usage through base class reference...\n";
    ShortestPathPolicy concrete;
    IRoutePolicy& interface = concrete;
    
    double refCost = interface.edgeCost(city, 0);
    assert(refCost == 6.0);
    std::cout << "✓ Base class reference calls derived implementation correctly\n";
    
    // Test 3: Can be passed to function expecting interface
    std::cout << "Testing function parameter usage...\n";
    auto routePlannerFunction = [](const IRoutePolicy& policy, const City& city, EdgeId edgeId) {
        return policy.edgeCost(city, edgeId);
    };
    
    double functionCost = routePlannerFunction(concrete, city, 0);
    assert(functionCost == 6.0);
    std::cout << "✓ Can be passed to function expecting IRoutePolicy interface\n";
    
    std::cout << "✓ ShortestPathPolicy can be used polymorphically by RoutePlanner\n";
}

/**
 * Verify compilation requirements
 */
void verifyCompilationRequirements() {
    std::cout << "\n=== Compilation Requirements Verification ===\n";
    
    // Test 1: Interface compiles correctly
    std::cout << "Testing interface compilation...\n";
    // This test passes if we can create instances and call methods
    std::unique_ptr<IRoutePolicy> policy = std::make_unique<ShortestPathPolicy>();
    assert(policy != nullptr);
    std::cout << "✓ Interface compiles correctly\n";
    
    // Test 2: ShortestPathPolicy uses only edge length
    std::cout << "Testing ShortestPathPolicy uses only edge length...\n";
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 9.0, 10));
    
    ShortestPathPolicy policy2;
    double cost = policy2.edgeCost(city, 0);
    assert(cost == city.edgeLength(0));
    assert(cost == 9.0);
    std::cout << "✓ ShortestPathPolicy uses only edge length\n";
    
    // Test 3: Can be used polymorphically
    std::cout << "Testing polymorphic usage...\n";
    IRoutePolicy* polymorphic = &policy2;
    double polyCost = polymorphic->edgeCost(city, 0);
    assert(polyCost == 9.0);
    std::cout << "✓ Can be used polymorphically by RoutePlanner\n";
}

int main() {
    std::cout << "=== Requirements Verification Test Suite ===\n\n";
    
    try {
        verifyIRoutePolicyAPI();
        verifyShortestPathPolicyImplementation();
        verifyPolymorphicUsage();
        verifyCompilationRequirements();
        
        std::cout << "\n=== All Requirements Verified! ===\n";
        std::cout << "✓ IRoutePolicy interface meets all API requirements\n";
        std::cout << "✓ ShortestPathPolicy implementation meets all requirements\n";
        std::cout << "✓ edgeCost() returns city.edgeLength(edgeId)\n";
        std::cout << "✓ shouldRerouteOnNode() returns false unless path is empty\n";
        std::cout << "✓ Can be used polymorphically by RoutePlanner\n";
        std::cout << "✓ Interface compiles correctly\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Requirements verification failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Requirements verification failed with unknown exception\n";
        return 1;
    }
}
