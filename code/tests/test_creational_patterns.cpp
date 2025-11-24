// code/tests/test_creational_patterns.cpp
#include <QtTest/QtTest>
#include "../patterns/IGridFactory.h"
#include "../patterns/RegularGridFactory.h"
#include "../patterns/RandomGridFactory.h"
#include "../patterns/RealWorldGridFactory.h"
#include "../patterns/PresetBuilder.h"
#include "../patterns/IPolicyFactory.h"
#include "../patterns/ShortestPathFactory.h"
#include "../patterns/CongestionAwareFactory.h"
#include "../patterns/PolicyRegistry.h"
#include "../core/City.h"
#include "../core/Preset.h"

/**
 * Comprehensive test suite for Creational Design Patterns:
 * 1. Factory Pattern (Grid Factories)
 * 2. Builder Pattern (Preset Builder)
 * 3. Abstract Factory Pattern (Policy Factory + Registry)
 */
class TestCreationalPatterns : public QObject {
    Q_OBJECT

private slots:
    // ========== Factory Pattern Tests ==========
    void testRegularGridFactory();
    void testRandomGridFactory();
    void testRealWorldGridFactory();
    void testFactoryPolymorphism();
    void testFactoryEdgeCases();
    void testFactoryGridConnectivity();
    
    // ========== Builder Pattern Tests ==========
    void testPresetBuilderBasic();
    void testPresetBuilderFluentInterface();
    void testPresetBuilderValidation();
    void testPresetBuilderReset();
    void testPresetBuilderBlockedEdges();
    void testPresetBuilderEdgeCases();
    void testPresetBuilderIsValid();
    void testPresetBuilderDefaultValues();
    
    // ========== Abstract Factory Pattern Tests ==========
    void testShortestPathFactory();
    void testCongestionAwareFactory();
    void testPolicyRegistrySingleton();
    void testPolicyRegistryCreateByName();
    void testPolicyRegistryCreateByType();
    void testPolicyRegistryGetAvailable();
    void testPolicyRegistryGetInfo();
    void testPolicyRegistryExtensibility();
    void testPolicyRegistryErrorHandling();
    void testPolicyFactoryPolymorphism();
    
    // ========== Integration Tests ==========
    void testAllPatternsIntegration();
    void testBuilderWithFactory();
    void testRegistryWithBuilder();
};

// ========== Factory Pattern Tests ==========

void TestCreationalPatterns::testRegularGridFactory() {
    RegularGridFactory factory;
    QCOMPARE(factory.getFactoryType(), std::string("RegularGrid"));
    
    auto city = factory.createGrid(3, 3);
    QVERIFY(city != nullptr);
    QCOMPARE(city->getNodeCount(), 9);  // 3x3 = 9 nodes
    
    // Regular grid should have all connections
    // Horizontal: 3 rows * 2 connections = 6 edges (bidirectional = 12)
    // Vertical: 3 cols * 2 connections = 6 edges (bidirectional = 12)
    // Total: 24 edges
    QVERIFY(city->getEdgeCount() > 0);
}

void TestCreationalPatterns::testRandomGridFactory() {
    RandomGridFactory factory(0.5, 42);  // 50% connection probability
    QCOMPARE(factory.getFactoryType(), std::string("RandomGrid"));
    
    auto city = factory.createGrid(5, 5);
    QVERIFY(city != nullptr);
    QCOMPARE(city->getNodeCount(), 25);
    
    // Random grid should have fewer edges than regular (due to probability)
    QVERIFY(city->getEdgeCount() > 0);
}

void TestCreationalPatterns::testRealWorldGridFactory() {
    RealWorldGridFactory factory;
    QCOMPARE(factory.getFactoryType(), std::string("RealWorldGrid"));
    
    auto city = factory.createGrid(4, 4);
    QVERIFY(city != nullptr);
    QCOMPARE(city->getNodeCount(), 16);
    QVERIFY(city->getEdgeCount() > 0);
}

void TestCreationalPatterns::testFactoryPolymorphism() {
    // Test that factories can be used polymorphically
    std::vector<std::unique_ptr<IGridFactory>> factories;
    factories.push_back(std::make_unique<RegularGridFactory>());
    factories.push_back(std::make_unique<RandomGridFactory>(0.7));
    factories.push_back(std::make_unique<RealWorldGridFactory>());
    
    for (auto& factory : factories) {
        auto city = factory->createGrid(3, 3);
        QVERIFY(city != nullptr);
        QCOMPARE(city->getNodeCount(), 9);
        QVERIFY(!factory->getFactoryType().empty());
    }
}

// ========== Builder Pattern Tests ==========

void TestCreationalPatterns::testPresetBuilderBasic() {
    PresetBuilder builder;
    Preset preset = builder
        .setName("Test Preset")
        .setGridSize(5, 5)
        .setAgentCount(10)
        .setPolicy(PolicyType::SHORTEST_PATH)
        .setTickInterval(100)
        .build();
    
    QCOMPARE(preset.getName(), std::string("Test Preset"));
    QCOMPARE(preset.getRows(), 5);
    QCOMPARE(preset.getCols(), 5);
    QCOMPARE(preset.getAgentCount(), 10);
    QCOMPARE(preset.getPolicy(), PolicyType::SHORTEST_PATH);
    QCOMPARE(preset.getTickMs(), 100);
    QVERIFY(preset.validate());
}

void TestCreationalPatterns::testPresetBuilderFluentInterface() {
    // Test method chaining
    PresetBuilder builder;
    Preset preset = builder
        .setName("Fluent Test")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .setPolicy(PolicyType::CONGESTION_AWARE)
        .addBlockedEdge(0, 1)
        .addBlockedEdge(1, 2)
        .setTickInterval(50)
        .build();
    
    QCOMPARE(preset.getName(), std::string("Fluent Test"));
    QCOMPARE(preset.getBlockedEdges().size(), size_t(2));
}

void TestCreationalPatterns::testPresetBuilderValidation() {
    PresetBuilder builder;
    
    // Valid preset should build successfully
    QVERIFY(builder
        .setName("Valid")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .isValid());
    
    // Invalid preset (zero rows) should fail
    bool exceptionThrown = false;
    try {
        builder.reset()
            .setName("Invalid")
            .setGridSize(0, 3)  // Invalid
            .setAgentCount(5)
            .build();
    } catch (const std::runtime_error&) {
        exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);
}

void TestCreationalPatterns::testPresetBuilderReset() {
    PresetBuilder builder;
    builder.setName("First")
           .setGridSize(5, 5)
           .setAgentCount(10);
    
    builder.reset();
    
    Preset preset = builder
        .setName("Second")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .build();
    
    QCOMPARE(preset.getName(), std::string("Second"));
    QCOMPARE(preset.getRows(), 3);
    QCOMPARE(preset.getAgentCount(), 5);
}

void TestCreationalPatterns::testPresetBuilderBlockedEdges() {
    PresetBuilder builder;
    std::vector<std::pair<NodeId, NodeId>> edges = {{0, 1}, {1, 2}, {2, 3}};
    
    Preset preset = builder
        .setName("Blocked Edges Test")
        .setGridSize(5, 5)
        .setAgentCount(5)
        .addBlockedEdge(10, 11)
        .addBlockedEdges(edges)
        .build();
    
    QCOMPARE(preset.getBlockedEdges().size(), size_t(4));  // 1 + 3
}

// ========== Abstract Factory Pattern Tests ==========

void TestCreationalPatterns::testShortestPathFactory() {
    ShortestPathFactory factory;
    QCOMPARE(factory.getPolicyName(), std::string("ShortestPath"));
    QVERIFY(!factory.getDescription().empty());
    QCOMPARE(factory.getPolicyType(), PolicyType::SHORTEST_PATH);
    
    auto policy = factory.createPolicy();
    QVERIFY(policy != nullptr);
}

void TestCreationalPatterns::testCongestionAwareFactory() {
    CongestionAwareFactory factory;
    QCOMPARE(factory.getPolicyName(), std::string("CongestionAware"));
    QVERIFY(!factory.getDescription().empty());
    QCOMPARE(factory.getPolicyType(), PolicyType::CONGESTION_AWARE);
    
    auto policy = factory.createPolicy();
    QVERIFY(policy != nullptr);
}

void TestCreationalPatterns::testPolicyRegistrySingleton() {
    PolicyRegistry& reg1 = PolicyRegistry::getInstance();
    PolicyRegistry& reg2 = PolicyRegistry::getInstance();
    
    // Should be the same instance
    QCOMPARE(&reg1, &reg2);
}

void TestCreationalPatterns::testPolicyRegistryCreateByName() {
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    
    auto policy1 = registry.createPolicy("ShortestPath");
    QVERIFY(policy1 != nullptr);
    
    auto policy2 = registry.createPolicy("CongestionAware");
    QVERIFY(policy2 != nullptr);
    
    // Should throw for unknown policy
    bool exceptionThrown = false;
    try {
        registry.createPolicy("UnknownPolicy");
    } catch (const std::runtime_error&) {
        exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);
}

void TestCreationalPatterns::testPolicyRegistryCreateByType() {
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    
    auto policy1 = registry.createPolicy(PolicyType::SHORTEST_PATH);
    QVERIFY(policy1 != nullptr);
    
    auto policy2 = registry.createPolicy(PolicyType::CONGESTION_AWARE);
    QVERIFY(policy2 != nullptr);
}

void TestCreationalPatterns::testPolicyRegistryGetAvailable() {
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    auto policies = registry.getAvailablePolicies();
    
    QVERIFY(policies.size() >= 2);
    QVERIFY(std::find(policies.begin(), policies.end(), "ShortestPath") != policies.end());
    QVERIFY(std::find(policies.begin(), policies.end(), "CongestionAware") != policies.end());
}

void TestCreationalPatterns::testPolicyRegistryGetInfo() {
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    
    auto info = registry.getPolicyInfo("ShortestPath");
    QCOMPARE(info.first, std::string("ShortestPath"));
    QVERIFY(!info.second.empty());
    
    QVERIFY(registry.isRegistered("ShortestPath"));
    QVERIFY(registry.isRegistered("CongestionAware"));
    QVERIFY(!registry.isRegistered("UnknownPolicy"));
}

void TestCreationalPatterns::testPolicyRegistryExtensibility() {
    // This test demonstrates how new policies can be added
    // without modifying existing code (Open/Closed Principle)
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    
    // Registry should have at least the default policies
    QVERIFY(registry.isRegistered("ShortestPath"));
    QVERIFY(registry.isRegistered("CongestionAware"));
    
    // In a real scenario, you could register a new policy like this:
    // registry.registerFactory("PredictivePolicy", std::make_unique<PredictivePolicyFactory>());
    // This demonstrates extensibility without modifying existing code
}

void TestCreationalPatterns::testFactoryEdgeCases() {
    // Test edge cases for factories
    
    // 1x1 grid (minimal case)
    RegularGridFactory factory1;
    auto city1 = factory1.createGrid(1, 1);
    QCOMPARE(city1->getNodeCount(), 1);
    QCOMPARE(city1->getEdgeCount(), 0);  // No edges in 1x1 grid
    
    // Large grid
    auto city2 = factory1.createGrid(10, 10);
    QCOMPARE(city2->getNodeCount(), 100);
    QVERIFY(city2->getEdgeCount() > 0);
    
    // Rectangular grid (not square)
    auto city3 = factory1.createGrid(3, 5);
    QCOMPARE(city3->getNodeCount(), 15);
    QVERIFY(city3->getEdgeCount() > 0);
    
    // Random factory with different probabilities
    RandomGridFactory factory2(1.0, 42);  // 100% connection
    auto city4 = factory2.createGrid(3, 3);
    QVERIFY(city4->getEdgeCount() > 0);
    
    RandomGridFactory factory3(0.0, 42);  // 0% connection
    auto city5 = factory3.createGrid(3, 3);
    QCOMPARE(city5->getNodeCount(), 9);
    // Should have no edges (0% probability)
}

void TestCreationalPatterns::testFactoryGridConnectivity() {
    // Test that grids have proper connectivity
    RegularGridFactory factory;
    auto city = factory.createGrid(5, 5);
    
    // Check that nodes have neighbors
    for (int i = 0; i < city->getNodeCount(); ++i) {
        auto neighbors = city->neighbors(i);
        // Most nodes should have neighbors (except edge cases)
        if (i > 0 && i < city->getNodeCount() - 1) {
            QVERIFY(neighbors.size() > 0);
        }
    }
}

void TestCreationalPatterns::testPresetBuilderEdgeCases() {
    PresetBuilder builder;
    
    // Test minimum valid preset
    Preset minPreset = builder
        .setName("Min")
        .setGridSize(1, 1)
        .setAgentCount(0)
        .build();
    QVERIFY(minPreset.validate());
    
    // Test maximum reasonable preset
    Preset maxPreset = builder.reset()
        .setName("Max")
        .setGridSize(100, 100)
        .setAgentCount(1000)
        .build();
    QVERIFY(maxPreset.validate());
    
    // Test with many blocked edges
    builder.reset();
    for (int i = 0; i < 10; ++i) {
        builder.addBlockedEdge(i, i + 1);
    }
    Preset manyBlocked = builder
        .setName("Many Blocked")
        .setGridSize(10, 10)
        .setAgentCount(5)
        .build();
    QCOMPARE(manyBlocked.getBlockedEdges().size(), size_t(10));
}

void TestCreationalPatterns::testPresetBuilderIsValid() {
    PresetBuilder builder;
    
    // Valid configuration
    QVERIFY(builder
        .setName("Valid")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .isValid());
    
    // Invalid: zero rows
    QVERIFY(!builder.reset()
        .setName("Invalid")
        .setGridSize(0, 3)
        .setAgentCount(5)
        .isValid());
    
    // Invalid: negative agent count
    QVERIFY(!builder.reset()
        .setName("Invalid")
        .setGridSize(3, 3)
        .setAgentCount(-1)
        .isValid());
    
    // Invalid: zero tick interval
    QVERIFY(!builder.reset()
        .setName("Invalid")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .setTickInterval(0)
        .isValid());
}

void TestCreationalPatterns::testPresetBuilderDefaultValues() {
    PresetBuilder builder;
    
    // Build with minimal configuration
    Preset preset = builder
        .setName("Default Test")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .build();
    
    // Check default values
    QCOMPARE(preset.getPolicy(), PolicyType::SHORTEST_PATH);  // Default policy
    QCOMPARE(preset.getTickMs(), 100);  // Default tick interval
    QVERIFY(preset.getBlockedEdges().empty());  // Default: no blocked edges
}

void TestCreationalPatterns::testPolicyRegistryErrorHandling() {
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    
    // Test getPolicyInfo with invalid name
    bool exceptionThrown = false;
    try {
        registry.getPolicyInfo("NonExistentPolicy");
    } catch (const std::runtime_error&) {
        exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);
    
    // Test createPolicy with invalid name
    exceptionThrown = false;
    try {
        registry.createPolicy("InvalidPolicyName");
    } catch (const std::runtime_error&) {
        exceptionThrown = true;
    }
    QVERIFY(exceptionThrown);
}

void TestCreationalPatterns::testPolicyFactoryPolymorphism() {
    // Test that factories can be used polymorphically
    std::vector<std::unique_ptr<IPolicyFactory>> factories;
    factories.push_back(std::make_unique<ShortestPathFactory>());
    factories.push_back(std::make_unique<CongestionAwareFactory>());
    
    for (auto& factory : factories) {
        auto policy = factory->createPolicy();
        QVERIFY(policy != nullptr);
        QVERIFY(!factory->getPolicyName().empty());
        QVERIFY(!factory->getDescription().empty());
    }
}

void TestCreationalPatterns::testAllPatternsIntegration() {
    // Integration test: Use all three patterns together
    
    // 1. Use Builder to create preset
    Preset preset = PresetBuilder()
        .setName("Integration Test")
        .setGridSize(5, 5)
        .setAgentCount(10)
        .setPolicy(PolicyType::CONGESTION_AWARE)
        .addBlockedEdge(0, 1)
        .setTickInterval(100)
        .build();
    
    QVERIFY(preset.validate());
    
    // 2. Use Factory to create grid
    RegularGridFactory gridFactory;
    auto city = gridFactory.createGrid(preset.getRows(), preset.getCols());
    QVERIFY(city != nullptr);
    QCOMPARE(city->getNodeCount(), 25);
    
    // 3. Use Registry to create policy
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    auto policy = registry.createPolicy(preset.getPolicy());
    QVERIFY(policy != nullptr);
    
    // 4. Verify all components work together
    QVERIFY(city->getNodeCount() > 0);
    QVERIFY(policy != nullptr);
    QCOMPARE(preset.getPolicy(), PolicyType::CONGESTION_AWARE);
}

void TestCreationalPatterns::testBuilderWithFactory() {
    // Test Builder and Factory working together
    
    // Create preset with Builder
    Preset preset = PresetBuilder()
        .setName("Builder-Factory Test")
        .setGridSize(4, 4)
        .setAgentCount(8)
        .build();
    
    // Create grid with Factory
    RegularGridFactory factory;
    auto city = factory.createGrid(preset.getRows(), preset.getCols());
    
    // Verify compatibility
    int totalNodes = preset.getRows() * preset.getCols();
    QCOMPARE(city->getNodeCount(), totalNodes);
    QVERIFY(preset.getAgentCount() <= totalNodes);
}

void TestCreationalPatterns::testRegistryWithBuilder() {
    // Test Registry and Builder working together
    
    PolicyRegistry& registry = PolicyRegistry::getInstance();
    
    // Create preset with Builder using policy from Registry
    Preset preset1 = PresetBuilder()
        .setName("Registry-Builder Test 1")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .setPolicy(PolicyType::SHORTEST_PATH)
        .build();
    
    // Create policy from Registry
    auto policy1 = registry.createPolicy(preset1.getPolicy());
    QVERIFY(policy1 != nullptr);
    
    // Create another preset with different policy
    Preset preset2 = PresetBuilder()
        .setName("Registry-Builder Test 2")
        .setGridSize(3, 3)
        .setAgentCount(5)
        .setPolicy(PolicyType::CONGESTION_AWARE)
        .build();
    
    auto policy2 = registry.createPolicy(preset2.getPolicy());
    QVERIFY(policy2 != nullptr);
    
    // Verify policies are different
    QVERIFY(preset1.getPolicy() != preset2.getPolicy());
}

QTEST_MAIN(TestCreationalPatterns)
#include "test_creational_patterns.moc"

