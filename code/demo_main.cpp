// code/demo_main.cpp
// Command-line demo for GridlockLondon simulation
// Runs simulation and prints KPIs

#include <iostream>
#include <iomanip>
#include <string>
#include "core/SimulationController.h"
#include "core/Preset.h"
#include "core/Metrics.h"
#include "core/Agent.h"

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printMetrics(Metrics* metrics, int tick, bool detailed = false) {
    if (!metrics) return;
    
    if (detailed) {
        std::cout << "\n--- Tick " << tick << " ---\n";
    }
    
    double avgTripTime = metrics->averageTripTime();
    int throughput = metrics->totalThroughput();
    int maxEdgeLoad = metrics->getMaxEdgeLoad();
    
    std::cout << std::fixed << std::setprecision(2);
    
    if (avgTripTime > 0) {
        std::cout << "  Average Trip Time: " << avgTripTime << " ticks\n";
    } else {
        std::cout << "  Average Trip Time: N/A (no completed trips)\n";
    }
    
    std::cout << "  Total Throughput: " << throughput << " agents\n";
    std::cout << "  Max Edge Load: " << maxEdgeLoad << "\n";
    
    if (detailed) {
        std::cout << "-------------------\n";
    }
}

void runSimulation(SimulationController& controller, const Preset& preset, 
                   int numTicks, bool printEachTick = false) {
    // Load the preset
    try {
        controller.loadPreset(preset);
        std::cout << "✓ Preset loaded: " << preset.getName() 
                  << " (" << preset.getRows() << "x" << preset.getCols() 
                  << " grid, " << preset.getAgentCount() << " agents)\n";
    } catch (const std::exception& e) {
        std::cerr << "✗ Failed to load preset: " << e.what() << "\n";
        return;
    }
    
    // Display policy
    std::string policyName = (preset.getPolicy() == PolicyType::SHORTEST_PATH) 
                             ? "SHORTEST_PATH" : "CONGESTION_AWARE";
    std::cout << "  Policy: " << policyName << "\n";
    
    // Start simulation
    controller.start();
    std::cout << "\nStarting simulation for " << numTicks << " ticks...\n";
    
    if (printEachTick) {
        std::cout << "\nMetrics during simulation:\n";
    }
    
    // Run ticks
    for (int i = 1; i <= numTicks; ++i) {
        controller.tick();
        
        if (printEachTick && (i % 10 == 0 || i == 1 || i == numTicks)) {
            // Print every 10 ticks, plus first and last
            printMetrics(controller.getMetrics(), i, true);
        }
    }
    
    // Print final metrics
    std::cout << "\n";
    printHeader("Final Metrics");
    printMetrics(controller.getMetrics(), numTicks, false);
    
    // Show agent status
    std::vector<Agent*>& agents = controller.getAgents();
    int arrived = 0;
    int inProgress = 0;
    
    for (auto* agent : agents) {
        if (agent->hasArrived()) {
            arrived++;
        } else {
            inProgress++;
        }
    }
    
    std::cout << "\nAgent Status:\n";
    std::cout << "  Arrived: " << arrived << "\n";
    std::cout << "  In Progress: " << inProgress << "\n";
}

Preset createDemoPreset(int rows, int cols, int agentCount, 
                        PolicyType policy, const std::string& name) {
    Preset preset;
    preset.setName(name);
    preset.setRows(rows);
    preset.setCols(cols);
    preset.setAgentCount(agentCount);
    preset.setTickMs(100);
    preset.setPolicy(policy);
    
    // No blocked edges for simpler demo
    std::vector<std::pair<NodeId, NodeId>> blocked;
    preset.setBlockedEdges(blocked);
    
    return preset;
}

int main(int argc, char* argv[]) {
    std::cout << "\n";
    printHeader("GridlockLondon Simulation Demo");
    std::cout << "\n";
    
    // Parse command line arguments (optional)
    int gridSize = 3;
    int agentCount = 7;
    int numTicks = 75;
    bool printEachTick = false;
    bool comparePolicies = false;
    
    if (argc > 1) {
        gridSize = std::stoi(argv[1]);
    }
    if (argc > 2) {
        agentCount = std::stoi(argv[2]);
    }
    if (argc > 3) {
        numTicks = std::stoi(argv[3]);
    }
    if (argc > 4 && std::string(argv[4]) == "--verbose") {
        printEachTick = true;
    }
    if (argc > 4 && std::string(argv[4]) == "--compare") {
        comparePolicies = true;
    }
    
    // Validate parameters
    if (gridSize < 3 || gridSize > 10) {
        std::cerr << "Grid size should be between 3 and 10\n";
        return 1;
    }
    if (agentCount < 1 || agentCount > 20) {
        std::cerr << "Agent count should be between 1 and 20\n";
        return 1;
    }
    if (numTicks < 1 || numTicks > 500) {
        std::cerr << "Number of ticks should be between 1 and 500\n";
        return 1;
    }
    
    SimulationController controller;
    
    if (comparePolicies) {
        // Compare both policies
        printHeader("Running with SHORTEST_PATH Policy");
        Preset preset1 = createDemoPreset(gridSize, gridSize, agentCount, 
                                         PolicyType::SHORTEST_PATH, 
                                         "demo_" + std::to_string(gridSize) + "x" + std::to_string(gridSize));
        runSimulation(controller, preset1, numTicks, printEachTick);
        
        printHeader("Running with CONGESTION_AWARE Policy");
        Preset preset2 = createDemoPreset(gridSize, gridSize, agentCount, 
                                         PolicyType::CONGESTION_AWARE, 
                                         "demo_" + std::to_string(gridSize) + "x" + std::to_string(gridSize));
        runSimulation(controller, preset2, numTicks, printEachTick);
    } else {
        // Run with default policy (SHORTEST_PATH)
        Preset preset = createDemoPreset(gridSize, gridSize, agentCount, 
                                        PolicyType::SHORTEST_PATH, 
                                        "demo_" + std::to_string(gridSize) + "x" + std::to_string(gridSize));
        runSimulation(controller, preset, numTicks, printEachTick);
    }
    
    std::cout << "\n";
    printHeader("Demo Complete");
    std::cout << "\nUsage: " << argv[0] << " [gridSize] [agentCount] [numTicks] [--verbose|--compare]\n";
    std::cout << "  Example: " << argv[0] << " 3 7 75 --compare\n";
    std::cout << "  Example: " << argv[0] << " 5 10 100 --verbose\n";
    std::cout << "\n";
    
    return 0;
}

