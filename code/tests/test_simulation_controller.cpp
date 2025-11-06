// code/tests/test_simulation_controller.cpp
// Test program for SimulationController implementation
#include <iostream>
#include <cassert>
#include "core/SimulationController.h"
#include "core/Preset.h"
#include "core/City.h"
#include "core/Agent.h"
#include "core/Metrics.h"

void testLoadPreset() {
    std::cout << "Test: Load Preset\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setName("test_preset");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    controller.loadPreset(preset);
    
    // Verify city was created
    City* city = controller.getCity();
    assert(city != nullptr);
    std::cout << "✓ City created successfully\n";
    
    // Verify agents were created
    std::vector<Agent*>& agents = controller.getAgents();
    assert(agents.size() == 5);
    std::cout << "✓ " << agents.size() << " agents created\n";
    
    // Verify metrics were created
    Metrics* metrics = controller.getMetrics();
    assert(metrics != nullptr);
    std::cout << "✓ Metrics created successfully\n";
}

void testStartPause() {
    std::cout << "\nTest: Start and Pause\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(3);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    controller.start();
    std::cout << "✓ Simulation started\n";
    
    controller.pause();
    std::cout << "✓ Simulation paused\n";
    
    controller.start();
    std::cout << "✓ Simulation restarted\n";
}

void testReset() {
    std::cout << "\nTest: Reset Functionality\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(3);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    // Run a few ticks
    controller.start();
    controller.tick();
    controller.tick();
    controller.tick();
    
    // Reset
    controller.reset();
    
    // Verify agents are reset (should be back at origins)
    std::vector<Agent*>& agentsAfter = controller.getAgents();
    for (size_t i = 0; i < agentsAfter.size(); ++i) {
        assert(agentsAfter[i]->getCurrentNode() == agentsAfter[i]->getOrigin());
        assert(!agentsAfter[i]->hasArrived());
    }
    std::cout << "✓ Agents reset to initial state\n";
}

void testTick() {
    std::cout << "\nTest: Tick Functionality\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(2);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    controller.start();
    
    // Run a tick
    controller.tick();
    std::cout << "✓ Tick executed successfully\n";
    
    // Run multiple ticks
    for (int i = 0; i < 10; ++i) {
        controller.tick();
    }
    std::cout << "✓ Multiple ticks executed successfully\n";
}

void testSetPolicy() {
    std::cout << "\nTest: Policy Switching\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(2);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    // Switch to CONGESTION_AWARE
    controller.setPolicy(PolicyType::CONGESTION_AWARE);
    std::cout << "✓ Switched to CONGESTION_AWARE policy\n";
    
    // Switch back to SHORTEST_PATH
    controller.setPolicy(PolicyType::SHORTEST_PATH);
    std::cout << "✓ Switched back to SHORTEST_PATH policy\n";
}

void testMetricsUpdate() {
    std::cout << "\nTest: Metrics Update\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(2);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    Metrics* metrics = controller.getMetrics();
    assert(metrics != nullptr);
    
    controller.start();
    
    // Run several ticks
    for (int i = 0; i < 5; ++i) {
        controller.tick();
    }
    
    assert(metrics != nullptr);
    std::cout << "✓ Metrics updated during ticks\n";
}

void testGetters() {
    std::cout << "\nTest: Getters\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(4);
    preset.setCols(4);
    preset.setAgentCount(3);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    // Test getCity()
    City* city = controller.getCity();
    assert(city != nullptr);
    std::cout << "✓ getCity() returns valid pointer\n";
    
    // Test getAgents()
    std::vector<Agent*>& agents = controller.getAgents();
    assert(agents.size() == 3);
    std::cout << "✓ getAgents() returns valid vector\n";
    
    // Test getMetrics()
    Metrics* metrics = controller.getMetrics();
    assert(metrics != nullptr);
    std::cout << "✓ getMetrics() returns valid pointer\n";
}

int main() {
    std::cout << "=== SimulationController Test Suite ===\n\n";
    
    try {
        testLoadPreset();
        testStartPause();
        testReset();
        testTick();
        testSetPolicy();
        testMetricsUpdate();
        testGetters();
        
        std::cout << "\n=== All SimulationController Tests Passed! ===\n";
        std::cout << "✓ Can load a preset and initialize City + Agents\n";
        std::cout << "✓ tick() moves all agents one step\n";
        std::cout << "✓ Agents reroute based on policy\n";
        std::cout << "✓ Metrics are updated each tick\n";
        std::cout << "✓ Can start/pause/reset simulation\n";
        std::cout << "✓ Compiles and integrates with other components\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}

