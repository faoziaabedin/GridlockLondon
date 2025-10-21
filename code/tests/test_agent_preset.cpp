// code/tests/test_agent_preset.cpp
// Test program for Agent and Preset classes
#include <iostream>
#include <cassert>
#include "core/Agent.h"
#include "core/Preset.h"
#include "core/City.h"

void testPreset() {
    std::cout << "Test: Preset Validation\n";
    
    Preset preset;
    preset.setName("test_preset");
    preset.setRows(5);
    preset.setCols(5);
    preset.setAgentCount(10);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    assert(preset.validate() == true);
    std::cout << "✓ Valid preset passes validation\n";
    
    // Test invalid preset
    Preset invalid;
    invalid.setRows(0);
    invalid.setCols(5);
    assert(invalid.validate() == false);
    std::cout << "✓ Invalid preset (0 rows) fails validation\n";
    
    // Test getters
    assert(preset.getName() == "test_preset");
    assert(preset.getRows() == 5);
    assert(preset.getCols() == 5);
    assert(preset.getAgentCount() == 10);
    assert(preset.getTickMs() == 100);
    assert(preset.getPolicy() == PolicyType::SHORTEST_PATH);
    std::cout << "✓ All getters return correct values\n";
}

void testAgent() {
    std::cout << "\nTest: Agent Basics\n";
    
    Agent agent(1, 0, 8);
    
    assert(agent.getId() == 1);
    assert(agent.getOrigin() == 0);
    assert(agent.getDestination() == 8);
    assert(agent.getCurrentNode() == 0);
    assert(agent.needsRoute() == true);
    assert(agent.hasArrived() == false);
    std::cout << "✓ Agent initialized correctly\n";
    
    // Set a path
    std::deque<EdgeId> path = {0, 1, 2};
    agent.setPath(path);
    assert(agent.needsRoute() == false);
    assert(agent.getPath().size() == 3);
    std::cout << "✓ Agent path set correctly\n";
}

void testAgentMovement() {
    std::cout << "\nTest: Agent Movement\n";
    
    // Create a simple 2-node city with one edge
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 1.0, 5)); // Edge from node 0 to 1, capacity 5
    
    // Create agent at node 0, destination node 1
    Agent agent(1, 0, 1);
    std::deque<EdgeId> path = {0}; // Take edge 0
    agent.setPath(path);
    
    // Initial state
    assert(city.occupancy(0) == 0);
    assert(agent.getCurrentNode() == 0);
    assert(!agent.getCurrentEdge().has_value());
    std::cout << "✓ Initial state: agent at node 0, not on edge, edge empty\n";
    
    // Step 1: Agent moves onto edge (takes edge from path, increments occupancy)
    agent.step(city);
    assert(city.occupancy(0) == 1);
    assert(agent.getCurrentEdge().has_value());
    assert(agent.getCurrentEdge().value() == 0);
    assert(agent.getCurrentNode() == 0); // Still at origin node
    assert(!agent.hasArrived());
    std::cout << "✓ Step 1: agent on edge 0, occupancy = 1, still at node 0\n";
    
    // Step 2: Agent finishes traversing edge (moves to destination, decrements occupancy)
    agent.step(city);
    std::cout << "  DEBUG: After step 2, occupancy = " << city.occupancy(0) << "\n";
    std::cout << "  DEBUG: Agent on edge? " << (agent.getCurrentEdge().has_value() ? "yes" : "no") << "\n";
    std::cout << "  DEBUG: Agent at node: " << agent.getCurrentNode() << "\n";
    std::cout << "  DEBUG: Agent arrived? " << (agent.hasArrived() ? "yes" : "no") << "\n";
    assert(city.occupancy(0) == 0); // Agent left the edge
    assert(!agent.getCurrentEdge().has_value()); // No longer on edge
    assert(agent.getCurrentNode() == 1); // Now at destination
    assert(agent.hasArrived() == true);
    std::cout << "✓ Step 2: agent arrived at node 1, edge empty, occupancy = 0\n";
    
    // Step 3: Agent doesn't move after arrival
    agent.step(city);
    assert(agent.getCurrentNode() == 1);
    assert(city.occupancy(0) == 0);
    std::cout << "✓ Step 3: agent stays at destination\n";
}

void testCapacityConstraint() {
    std::cout << "\nTest: Capacity Constraint\n";
    
    City city;
    city.addNode(Node(0, 0, 0));
    city.addNode(Node(1, 0, 1));
    city.addEdge(Edge(0, 0, 1, 1.0, 2)); // Capacity = 2
    
    Agent agent1(1, 0, 1);
    Agent agent2(2, 0, 1);
    Agent agent3(3, 0, 1);
    
    std::deque<EdgeId> path = {0};
    agent1.setPath(path);
    agent2.setPath(path);
    agent3.setPath(path);
    
    // Agent 1 enters edge
    agent1.step(city);
    assert(city.occupancy(0) == 1);
    std::cout << "✓ Agent 1 enters edge, occupancy = 1\n";
    
    // Agent 2 enters edge
    agent2.step(city);
    assert(city.occupancy(0) == 2);
    std::cout << "✓ Agent 2 enters edge, occupancy = 2 (at capacity)\n";
    
    // Agent 3 tries to enter but can't (capacity full)
    agent3.step(city);
    assert(city.occupancy(0) == 2);
    assert(agent3.getCurrentNode() == 0); // Still at origin
    assert(!agent3.getCurrentEdge().has_value()); // Not on edge
    std::cout << "✓ Agent 3 blocked (capacity full), waits at node 0\n";
    
    // Agent 1 finishes, frees space
    agent1.step(city);
    assert(city.occupancy(0) == 1);
    std::cout << "✓ Agent 1 exits, occupancy = 1\n";
    
    // Now agent 3 can enter
    agent3.step(city);
    assert(city.occupancy(0) == 2);
    std::cout << "✓ Agent 3 enters edge now that space is available\n";
}

int main() {
    std::cout << "=== Agent and Preset Test Suite ===\n\n";
    
    try {
        testPreset();
        testAgent();
        testAgentMovement();
        testCapacityConstraint();
        
        std::cout << "\n=== All Tests Passed! ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}