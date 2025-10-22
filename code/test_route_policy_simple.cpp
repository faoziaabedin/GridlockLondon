// Simple test to verify IRoutePolicy and ShortestPathPolicy work
#include <iostream>
#include <cassert>
#include <memory>
#include "core/IRoutePolicy.h"
#include "core/ShortestPathPolicy.h"
#include "core/City.h"
#include "core/Agent.h"
#include "core/Node.h"
#include "core/Edge.h"

int main() {
    std::cout << "=== Simple Route Policy Test ===\n";
    
    try {
        // Test 1: Create policy and verify interface
        std::cout << "Test 1: Creating ShortestPathPolicy...\n";
        ShortestPathPolicy policy;
        std::cout << "✓ ShortestPathPolicy created successfully\n";
        
        // Test 2: Create a simple city
        std::cout << "\nTest 2: Creating city with edges...\n";
        City city;
        city.addNode(Node(0, 0, 0));
        city.addNode(Node(1, 0, 1));
        city.addEdge(Edge(0, 0, 1, 5.0, 10));
        std::cout << "✓ City created with edge of length 5.0\n";
        
        // Test 3: Test edgeCost
        std::cout << "\nTest 3: Testing edgeCost...\n";
        double cost = policy.edgeCost(city, 0);
        std::cout << "Edge cost: " << cost << "\n";
        assert(cost == 5.0);
        std::cout << "✓ edgeCost returns correct value (5.0)\n";
        
        // Test 4: Test shouldRerouteOnNode
        std::cout << "\nTest 4: Testing shouldRerouteOnNode...\n";
        Agent agent(1, 0, 1);
        bool shouldReroute = policy.shouldRerouteOnNode(agent);
        std::cout << "Should reroute: " << (shouldReroute ? "true" : "false") << "\n";
        assert(shouldReroute == true); // Agent has no path initially
        std::cout << "✓ shouldRerouteOnNode returns true for agent with no path\n";
        
        // Test 5: Test polymorphic usage
        std::cout << "\nTest 5: Testing polymorphic usage...\n";
        std::unique_ptr<IRoutePolicy> interface = std::make_unique<ShortestPathPolicy>();
        double polymorphicCost = interface->edgeCost(city, 0);
        assert(polymorphicCost == 5.0);
        std::cout << "✓ Polymorphic usage works correctly\n";
        
        // Test 6: Test agent with path
        std::cout << "\nTest 6: Testing agent with path...\n";
        std::deque<EdgeId> path = {0};
        agent.setPath(path);
        bool shouldRerouteWithPath = policy.shouldRerouteOnNode(agent);
        std::cout << "Should reroute with path: " << (shouldRerouteWithPath ? "true" : "false") << "\n";
        assert(shouldRerouteWithPath == false);
        std::cout << "✓ Agent with path should NOT reroute\n";
        
        std::cout << "\n=== All Tests Passed! ===\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
}
