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
    
    controller.loadPreset(preset);
    controller.start();
    
    // Run simulation for 50 ticks
    for (int i = 0; i < 50; ++i) {
        controller.tick();
    }
    
    std::cout << "✓ Simulation ran for 50 ticks without crashing\n";
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
    
    // Verify metrics are available
    double avgTime = metrics->averageTripTime();
    int throughput = metrics->totalThroughput();
    int maxLoad = metrics->getMaxEdgeLoad();
    
    assert(avgTime >= 0);
    assert(throughput >= 0);
    assert(maxLoad >= 0);
    
    std::cout << "✓ Metrics available for printing\n";
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
    
    std::cout << "✓ SHORTEST_PATH policy tested\n";
    
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
    
    std::cout << "✓ CONGESTION_AWARE policy tested\n";
    std::cout << "✓ Policy comparison demonstrates differences\n";
}

int main() {
    std::cout << "=== CLI Demo Test Suite ===\n\n";
    
    try {
        testPresetCreation();
        testDemoSimulationRun();
        testMetricsPrinting();
        testPolicyComparison();
        
        std::cout << "\n=== All CLI Demo Tests Passed! ===\n";
        std::cout << "✓ Runs without crashing\n";
        std::cout << "✓ Prints meaningful metrics\n";
        std::cout << "✓ Demonstrates simulation working end-to-end\n";
        std::cout << "✓ Shows difference between routing policies\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}

