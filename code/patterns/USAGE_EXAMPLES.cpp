// code/patterns/USAGE_EXAMPLES.cpp
// Comprehensive usage examples for all three creational patterns

#include "IGridFactory.h"
#include "RegularGridFactory.h"
#include "RandomGridFactory.h"
#include "RealWorldGridFactory.h"
#include "PresetBuilder.h"
#include "PolicyRegistry.h"
#include "../core/City.h"
#include "../core/Preset.h"
#include "../core/RoutePlanner.h"
#include <iostream>
#include <memory>

// ========== Example 1: Factory Pattern ==========
void exampleFactoryPattern() {
    std::cout << "=== Factory Pattern Example ===\n";
    
    // Create different grid types using factories
    RegularGridFactory regularFactory;
    auto regularCity = regularFactory.createGrid(5, 5);
    std::cout << "Regular grid: " << regularCity->getNodeCount() << " nodes\n";
    
    RandomGridFactory randomFactory(0.7, 42);
    auto randomCity = randomFactory.createGrid(5, 5);
    std::cout << "Random grid: " << randomCity->getNodeCount() << " nodes\n";
    
    RealWorldGridFactory realWorldFactory;
    auto realWorldCity = realWorldFactory.createGrid(5, 5);
    std::cout << "Real-world grid: " << realWorldCity->getNodeCount() << " nodes\n";
    
    // Use polymorphically
    std::vector<std::unique_ptr<IGridFactory>> factories;
    factories.push_back(std::make_unique<RegularGridFactory>());
    factories.push_back(std::make_unique<RandomGridFactory>(0.5));
    factories.push_back(std::make_unique<RealWorldGridFactory>());
    
    for (auto& factory : factories) {
        auto city = factory->createGrid(3, 3);
        std::cout << "Factory type: " << factory->getFactoryType() 
                  << ", Nodes: " << city->getNodeCount() << "\n";
    }
}

// ========== Example 2: Builder Pattern ==========
void exampleBuilderPattern() {
    std::cout << "\n=== Builder Pattern Example ===\n";
    
    // Build a rush hour preset with fluent interface
    Preset rushHour = PresetBuilder()
        .setName("Rush Hour Simulation")
        .setGridSize(10, 10)
        .setAgentCount(50)
        .setPolicy(PolicyType::CONGESTION_AWARE)
        .addBlockedEdge(12, 13)
        .addBlockedEdge(23, 24)
        .setTickInterval(100)
        .build();
    
    std::cout << "Preset: " << rushHour.getName() << "\n";
    std::cout << "Grid: " << rushHour.getRows() << "x" << rushHour.getCols() << "\n";
    std::cout << "Agents: " << rushHour.getAgentCount() << "\n";
    std::cout << "Blocked edges: " << rushHour.getBlockedEdges().size() << "\n";
    
    // Build a comparison preset
    Preset comparison = PresetBuilder()
        .setName("Policy Comparison")
        .setGridSize(5, 5)
        .setAgentCount(20)
        .setPolicy(PolicyType::SHORTEST_PATH)
        .setTickInterval(150)
        .build();
    
    std::cout << "Comparison preset: " << comparison.getName() << "\n";
    
    // Add multiple blocked edges at once
    std::vector<std::pair<NodeId, NodeId>> blocked = {{0, 1}, {1, 2}, {2, 3}};
    Preset withMultipleBlocks = PresetBuilder()
        .setName("Multiple Blocked Edges")
        .setGridSize(5, 5)
        .setAgentCount(10)
        .addBlockedEdges(blocked)
        .build();
    
    std::cout << "Blocked edges: " << withMultipleBlocks.getBlockedEdges().size() << "\n";
}

// ========== Example 3: Abstract Factory Pattern ==========
void exampleAbstractFactoryPattern() {
    std::cout << "\n=== Abstract Factory Pattern Example ===\n";
    
    // Get registry instance (initializes with default policies)
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    
    // Create policy by name
    auto policy1 = registry.createPolicy("ShortestPath");
    std::cout << "Created policy: ShortestPath\n";
    
    auto policy2 = registry.createPolicy("CongestionAware");
    std::cout << "Created policy: CongestionAware\n";
    
    // Create policy by type
    auto policy3 = registry.createPolicy(PolicyType::SHORTEST_PATH);
    std::cout << "Created policy by type: SHORTEST_PATH\n";
    
    // List available policies
    auto available = registry.getAvailablePolicies();
    std::cout << "Available policies:\n";
    for (const auto& name : available) {
        auto info = registry.getPolicyInfo(name);
        std::cout << "  - " << info.first << ": " << info.second << "\n";
    }
    
    // Use in RoutePlanner
    RoutePlanner planner1(policy1.get());
    RoutePlanner planner2(policy2.get());
    std::cout << "Created RoutePlanners with different policies\n";
}

// ========== Example 4: All Patterns Together ==========
void exampleAllPatternsTogether() {
    std::cout << "\n=== All Patterns Together ===\n";
    
    // 1. Use Builder to create preset
    Preset preset = PresetBuilder()
        .setName("Integrated Example")
        .setGridSize(8, 8)
        .setAgentCount(30)
        .setPolicy(PolicyType::CONGESTION_AWARE)
        .addBlockedEdge(10, 11)
        .setTickInterval(100)
        .build();
    
    std::cout << "Created preset: " << preset.getName() << "\n";
    
    // 2. Use Factory to create grid
    RegularGridFactory factory;
    auto city = factory.createGrid(preset.getRows(), preset.getCols());
    std::cout << "Created city with " << city->getNodeCount() << " nodes\n";
    
    // 3. Use Registry to create policy
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    auto policy = registry.createPolicy(preset.getPolicy());
    std::cout << "Created policy: " << registry.getPolicyInfo("CongestionAware").first << "\n";
    
    // 4. Use all together in simulation
    RoutePlanner planner(policy.get());
    std::cout << "Integrated all patterns successfully!\n";
}

// ========== Example 5: Extensibility Demonstration ==========
void exampleExtensibility() {
    std::cout << "\n=== Extensibility Example ===\n";
    
    // Demonstrate how easy it is to extend:
    
    // 1. Add new grid factory (no code modification needed)
    // Just create a new class implementing IGridFactory
    // class HexagonalGridFactory : public IGridFactory { ... };
    
    // 2. Use Builder to create custom presets
    Preset custom = PresetBuilder()
        .setName("Custom Configuration")
        .setGridSize(7, 7)
        .setAgentCount(25)
        .setPolicy(PolicyType::SHORTEST_PATH)
        .build();
    
    std::cout << "Created custom preset without modifying Preset class\n";
    
    // 3. Register new policy (no code modification needed)
    // PolicyRegistry& registry = PolicyRegistry::getInstance();
    // registry.registerFactory("PredictivePolicy", 
    //     std::make_unique<PredictivePolicyFactory>());
    
    std::cout << "Can register new policies without modifying existing code\n";
}

int main() {
    exampleFactoryPattern();
    exampleBuilderPattern();
    exampleAbstractFactoryPattern();
    exampleAllPatternsTogether();
    exampleExtensibility();
    
    return 0;
}

