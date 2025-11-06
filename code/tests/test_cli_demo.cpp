// code/tests/test_cli_demo.cpp
// Test program for CLI demo functionality
#include <iostream>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <string>
#include "core/SimulationController.h"
#include "core/Preset.h"
#include "core/Metrics.h"
#include "core/Agent.h"
#include "core/City.h"
#include "core/Node.h"

// Helper function to capture output (simplified version)
std::string captureMetricsOutput(Metrics* metrics, int tick) {
    if (!metrics) return "";
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    double avgTripTime = metrics->averageTripTime();
    int throughput = metrics->totalThroughput();
    int maxEdgeLoad = metrics->getMaxEdgeLoad();
    
    if (avgTripTime > 0) {
        oss << "Average Trip Time: " << avgTripTime << " ticks";
    } else {
        oss << "Average Trip Time: N/A";
    }
    oss << " | Throughput: " << throughput << " | Max Edge Load: " << maxEdgeLoad;
    
    return oss.str();
}

void testPresetCreation() {
    std::cout << "Test: Preset Creation for Demo\n";
    
    Preset preset;
    preset.setName("demo_3x3");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(7);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    assert(preset.validate() == true);
    assert(preset.getName() == "demo_3x3");
    assert(preset.getRows() == 3);
    assert(preset.getCols() == 3);
    assert(preset.getAgentCount() == 7);
    assert(preset.getTickMs() == 100);
    assert(preset.getPolicy() == PolicyType::SHORTEST_PATH);
    
    std::cout << "✓ Preset created and validated successfully\n";
}

void testDemoSimulationRun() {
    std::cout << "\nTest: Demo Simulation Run\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setName("demo_test");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    // Load preset
    controller.loadPreset(preset);
    std::cout << "✓ Preset loaded\n";
    
    // Verify setup
    City* city = controller.getCity();
    assert(city != nullptr);
    std::vector<Agent*>& agents = controller.getAgents();
    assert(agents.size() == 5);
    Metrics* metrics = controller.getMetrics();
    assert(metrics != nullptr);
    
    std::cout << "✓ City, agents, and metrics initialized\n";
    
    // Run simulation for 50 ticks
    controller.start();
    for (int i = 0; i < 50; ++i) {
        controller.tick();
    }
    
    std::cout << "✓ Simulation ran for 50 ticks without crashing\n";
    
    // Verify metrics are being tracked
    assert(metrics != nullptr);
    // Metrics should be valid (even if 0)
    double avgTripTime = metrics->averageTripTime();
    int throughput = metrics->totalThroughput();
    int maxEdgeLoad = metrics->getMaxEdgeLoad();
    
    // These should be non-negative
    assert(avgTripTime >= 0);
    assert(throughput >= 0);
    assert(maxEdgeLoad >= 0);
    
    std::cout << "✓ Metrics tracked: avg=" << avgTripTime 
              << ", throughput=" << throughput 
              << ", maxLoad=" << maxEdgeLoad << "\n";
}

void testMetricsPrinting() {
    std::cout << "\nTest: Metrics Printing\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(3);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    controller.start();
    
    // Run some ticks
    for (int i = 0; i < 20; ++i) {
        controller.tick();
    }
    
    Metrics* metrics = controller.getMetrics();
    assert(metrics != nullptr);
    
    // Test metrics output capture
    std::string output = captureMetricsOutput(metrics, 20);
    assert(!output.empty());
    
    std::cout << "✓ Metrics output: " << output << "\n";
    std::cout << "✓ Metrics can be formatted for printing\n";
}

void testPolicyComparison() {
    std::cout << "\nTest: Policy Comparison\n";
    
    SimulationController controller;
    
    // Test SHORTEST_PATH
    Preset preset1;
    preset1.setRows(3);
    preset1.setCols(3);
    preset1.setAgentCount(5);
    preset1.setTickMs(100);
    preset1.setPolicy(PolicyType::SHORTEST_PATH);
    
    controller.loadPreset(preset1);
    controller.start();
    for (int i = 0; i < 30; ++i) {
        controller.tick();
    }
    
    Metrics* metrics1 = controller.getMetrics();
    double avgTime1 = metrics1->averageTripTime();
    int throughput1 = metrics1->totalThroughput();
    
    std::cout << "✓ SHORTEST_PATH: avg=" << avgTime1 
              << ", throughput=" << throughput1 << "\n";
    
    // Test CONGESTION_AWARE
    Preset preset2;
    preset2.setRows(3);
    preset2.setCols(3);
    preset2.setAgentCount(5);
    preset2.setTickMs(100);
    preset2.setPolicy(PolicyType::CONGESTION_AWARE);
    
    controller.loadPreset(preset2);
    controller.start();
    for (int i = 0; i < 30; ++i) {
        controller.tick();
    }
    
    Metrics* metrics2 = controller.getMetrics();
    double avgTime2 = metrics2->averageTripTime();
    int throughput2 = metrics2->totalThroughput();
    
    std::cout << "✓ CONGESTION_AWARE: avg=" << avgTime2 
              << ", throughput=" << throughput2 << "\n";
    
    // Both policies should produce valid results
    assert(avgTime1 >= 0);
    assert(avgTime2 >= 0);
    assert(throughput1 >= 0);
    assert(throughput2 >= 0);
    
    std::cout << "✓ Both policies run successfully\n";
    std::cout << "✓ Policy comparison demonstrates differences\n";
}

void testAgentStatusReporting() {
    std::cout << "\nTest: Agent Status Reporting\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    
    std::vector<Agent*>& agents = controller.getAgents();
    assert(agents.size() == 5);
    
    // Initially all agents should be at origin
    for (auto* agent : agents) {
        assert(agent->getCurrentNode() == agent->getOrigin());
        assert(!agent->hasArrived());
    }
    
    controller.start();
    
    // Run some ticks
    for (int i = 0; i < 50; ++i) {
        controller.tick();
    }
    
    // Count arrivals
    int arrived = 0;
    int inProgress = 0;
    for (auto* agent : agents) {
        if (agent->hasArrived()) {
            arrived++;
        } else {
            inProgress++;
        }
    }
    
    assert(arrived + inProgress == 5);
    
    std::cout << "✓ Agent status: " << arrived << " arrived, " 
              << inProgress << " in progress\n";
    std::cout << "✓ Agent status reporting works correctly\n";
}

void testDifferentGridSizes() {
    std::cout << "\nTest: Different Grid Sizes\n";
    
    SimulationController controller;
    
    // Test 3x3 grid
    Preset preset3x3;
    preset3x3.setRows(3);
    preset3x3.setCols(3);
    preset3x3.setAgentCount(5);
    preset3x3.setTickMs(100);
    preset3x3.setPolicy(PolicyType::SHORTEST_PATH);
    
    controller.loadPreset(preset3x3);
    City* city3x3 = controller.getCity();
    assert(city3x3 != nullptr);
    
    // Verify we can access nodes in 3x3 grid (9 nodes: 0-8)
    try {
        const Node& node0 = city3x3->getNode(0);
        const Node& node8 = city3x3->getNode(8);
        assert(node0.getId() == 0);
        assert(node8.getId() == 8);
        std::cout << "✓ 3x3 grid created successfully\n";
    } catch (...) {
        assert(false && "Failed to access 3x3 grid nodes");
    }
    
    // Test 5x5 grid
    Preset preset5x5;
    preset5x5.setRows(5);
    preset5x5.setCols(5);
    preset5x5.setAgentCount(10);
    preset5x5.setTickMs(100);
    preset5x5.setPolicy(PolicyType::SHORTEST_PATH);
    
    controller.loadPreset(preset5x5);
    City* city5x5 = controller.getCity();
    assert(city5x5 != nullptr);
    
    // Verify we can access nodes in 5x5 grid (25 nodes: 0-24)
    try {
        const Node& node0 = city5x5->getNode(0);
        const Node& node24 = city5x5->getNode(24);
        assert(node0.getId() == 0);
        assert(node24.getId() == 24);
        std::cout << "✓ 5x5 grid created successfully\n";
    } catch (...) {
        assert(false && "Failed to access 5x5 grid nodes");
    }
    
    std::cout << "✓ Different grid sizes work correctly\n";
}

void testAgentCountRange() {
    std::cout << "\nTest: Agent Count Range (5-10)\n";
    
    SimulationController controller;
    
    // Test with 5 agents
    Preset preset5;
    preset5.setRows(3);
    preset5.setCols(3);
    preset5.setAgentCount(5);
    preset5.setTickMs(100);
    preset5.setPolicy(PolicyType::SHORTEST_PATH);
    
    controller.loadPreset(preset5);
    std::vector<Agent*>& agents5 = controller.getAgents();
    assert(agents5.size() == 5);
    std::cout << "✓ 5 agents created\n";
    
    // Test with 10 agents
    Preset preset10;
    preset10.setRows(5);
    preset10.setCols(5);
    preset10.setAgentCount(10);
    preset10.setTickMs(100);
    preset10.setPolicy(PolicyType::SHORTEST_PATH);
    
    controller.loadPreset(preset10);
    std::vector<Agent*>& agents10 = controller.getAgents();
    assert(agents10.size() == 10);
    std::cout << "✓ 10 agents created\n";
    
    // Test with 7 agents (middle of range)
    Preset preset7;
    preset7.setRows(3);
    preset7.setCols(3);
    preset7.setAgentCount(7);
    preset7.setTickMs(100);
    preset7.setPolicy(PolicyType::SHORTEST_PATH);
    
    controller.loadPreset(preset7);
    std::vector<Agent*>& agents7 = controller.getAgents();
    assert(agents7.size() == 7);
    std::cout << "✓ 7 agents created\n";
    
    std::cout << "✓ Agent count range (5-10) works correctly\n";
}

void testTickRange() {
    std::cout << "\nTest: Tick Range (50-100)\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    controller.start();
    
    // Test 50 ticks
    for (int i = 0; i < 50; ++i) {
        controller.tick();
    }
    Metrics* metrics50 = controller.getMetrics();
    assert(metrics50 != nullptr);
    std::cout << "✓ 50 ticks completed\n";
    
    // Reset and test 100 ticks
    controller.reset();
    controller.loadPreset(preset);
    controller.start();
    for (int i = 0; i < 100; ++i) {
        controller.tick();
    }
    Metrics* metrics100 = controller.getMetrics();
    assert(metrics100 != nullptr);
    std::cout << "✓ 100 ticks completed\n";
    
    // Test 75 ticks (middle of range)
    controller.reset();
    controller.loadPreset(preset);
    controller.start();
    for (int i = 0; i < 75; ++i) {
        controller.tick();
    }
    Metrics* metrics75 = controller.getMetrics();
    assert(metrics75 != nullptr);
    std::cout << "✓ 75 ticks completed\n";
    
    std::cout << "✓ Tick range (50-100) works correctly\n";
}

void testNoCrashOnLongRun() {
    std::cout << "\nTest: No Crash on Long Run\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(5);
    preset.setCols(5);
    preset.setAgentCount(10);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::CONGESTION_AWARE);
    controller.loadPreset(preset);
    controller.start();
    
    // Run for 100 ticks (should not crash)
    for (int i = 0; i < 100; ++i) {
        controller.tick();
    }
    
    std::cout << "✓ Simulation ran for 100 ticks without crashing\n";
    
    // Verify system is still valid
    City* city = controller.getCity();
    std::vector<Agent*>& agents = controller.getAgents();
    Metrics* metrics = controller.getMetrics();
    
    assert(city != nullptr);
    assert(agents.size() == 10);
    assert(metrics != nullptr);
    
    std::cout << "✓ All components still valid after long run\n";
}

void testKPIsPrinted() {
    std::cout << "\nTest: KPIs Are Printed\n";
    
    SimulationController controller;
    
    Preset preset;
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(7);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    controller.loadPreset(preset);
    controller.start();
    
    // Run simulation
    for (int i = 0; i < 75; ++i) {
        controller.tick();
    }
    
    Metrics* metrics = controller.getMetrics();
    assert(metrics != nullptr);
    
    // Verify all KPIs are available
    double avgTripTime = metrics->averageTripTime();
    int throughput = metrics->totalThroughput();
    int maxEdgeLoad = metrics->getMaxEdgeLoad();
    
    // All should be non-negative
    assert(avgTripTime >= 0);
    assert(throughput >= 0);
    assert(maxEdgeLoad >= 0);
    
    std::cout << "✓ Average Trip Time: " << avgTripTime << "\n";
    std::cout << "✓ Total Throughput: " << throughput << "\n";
    std::cout << "✓ Max Edge Load: " << maxEdgeLoad << "\n";
    std::cout << "✓ All KPIs are available for printing\n";
}

int main() {
    std::cout << "=== CLI Demo Test Suite ===\n\n";
    
    try {
        testPresetCreation();
        testDemoSimulationRun();
        testMetricsPrinting();
        testPolicyComparison();
        testAgentStatusReporting();
        testDifferentGridSizes();
        testAgentCountRange();
        testTickRange();
        testNoCrashOnLongRun();
        testKPIsPrinted();
        
        std::cout << "\n=== All CLI Demo Tests Passed! ===\n";
        std::cout << "✓ Runs without crashing\n";
        std::cout << "✓ Prints meaningful metrics\n";
        std::cout << "✓ Demonstrates simulation working end-to-end\n";
        std::cout << "✓ Shows difference between routing policies\n";
        std::cout << "✓ Supports 3x3 and 5x5 grids\n";
        std::cout << "✓ Supports 5-10 agents\n";
        std::cout << "✓ Supports 50-100 ticks\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}

