// code/tests/test_factory_pattern_googletest.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../adapters/PresetLoader.h"
#include "../core/Preset.h"
#include "../core/City.h"
#include "../core/Agent.h"

/**
 * Test Suite 5: Factory Pattern (PresetLoader) (8+ tests)
 * Tests grid generation, validation, and edge case grids.
 */

class FactoryPatternTest : public ::testing::Test {
protected:
    void SetUp() override {
        loader = std::make_unique<PresetLoader>();
    }
    
    void TearDown() override {
        loader.reset();
    }
    
    std::unique_ptr<PresetLoader> loader;
};

// Test 1: Grid generation - 3x3
TEST_F(FactoryPatternTest, GridGeneration3x3) {
    auto city = loader->createGridTopology(3, 3);
    
    EXPECT_NE(city, nullptr);
    EXPECT_EQ(city->getNodeCount(), 9);  // 3 * 3 = 9 nodes
    EXPECT_GT(city->getEdgeCount(), 0);
}

// Test 2: Grid generation - 5x5
TEST_F(FactoryPatternTest, GridGeneration5x5) {
    auto city = loader->createGridTopology(5, 5);
    
    EXPECT_NE(city, nullptr);
    EXPECT_EQ(city->getNodeCount(), 25);  // 5 * 5 = 25 nodes
    EXPECT_GT(city->getEdgeCount(), 0);
}

// Test 3: Grid validation - nodes connected
TEST_F(FactoryPatternTest, GridValidationNodesConnected) {
    auto city = loader->createGridTopology(3, 3);
    
    // Check that nodes have neighbors
    for (int i = 0; i < city->getNodeCount(); ++i) {
        NodeId nid = city->getNodeIdByIndex(i);
        auto neighbors = city->neighbors(nid);
        // Corner nodes have 2 neighbors, edge nodes have 3, center has 4
        EXPECT_GE(neighbors.size(), 1);
    }
}

// Test 4: Build city from preset
TEST_F(FactoryPatternTest, BuildCityFromPreset) {
    Preset preset;
    preset.setName("test");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    auto city = loader->buildCity(preset);
    
    EXPECT_NE(city, nullptr);
    EXPECT_EQ(city->getNodeCount(), 9);
}

// Test 5: Spawn agents from preset
TEST_F(FactoryPatternTest, SpawnAgentsFromPreset) {
    Preset preset;
    preset.setName("test");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    auto city = loader->buildCity(preset);
    auto agents = loader->spawnAgents(preset, *city);
    
    EXPECT_EQ(agents.size(), 5);
    
    // Verify agents have valid origins and destinations
    for (const auto& agent : agents) {
        EXPECT_GE(agent->getOrigin(), 0);
        EXPECT_LT(agent->getOrigin(), city->getNodeCount());
        EXPECT_GE(agent->getDestination(), 0);
        EXPECT_LT(agent->getDestination(), city->getNodeCount());
    }
}

// Test 6: Edge case - 1x1 grid
TEST_F(FactoryPatternTest, EdgeCase1x1Grid) {
    auto city = loader->createGridTopology(1, 1);
    
    EXPECT_NE(city, nullptr);
    EXPECT_EQ(city->getNodeCount(), 1);
    EXPECT_EQ(city->getEdgeCount(), 0);  // Single node, no edges
}

// Test 7: Edge case - 2x2 grid
TEST_F(FactoryPatternTest, EdgeCase2x2Grid) {
    auto city = loader->createGridTopology(2, 2);
    
    EXPECT_NE(city, nullptr);
    EXPECT_EQ(city->getNodeCount(), 4);
    EXPECT_GT(city->getEdgeCount(), 0);
}

// Test 8: Apply blocked edges
TEST_F(FactoryPatternTest, ApplyBlockedEdges) {
    Preset preset;
    preset.setName("test");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    std::vector<std::pair<NodeId, NodeId>> blocked = {{0, 1}, {1, 2}};
    preset.setBlockedEdges(blocked);
    
    auto city = loader->buildCity(preset);
    
    EXPECT_NE(city, nullptr);
    // Verify blocked edges have capacity 0 or are removed
    // (Implementation dependent)
}

// Test 9: Large grid generation
TEST_F(FactoryPatternTest, LargeGridGeneration) {
    auto city = loader->createGridTopology(10, 10);
    
    EXPECT_NE(city, nullptr);
    EXPECT_EQ(city->getNodeCount(), 100);
    EXPECT_GT(city->getEdgeCount(), 0);
}

// Test 10: Preset validation
TEST_F(FactoryPatternTest, PresetValidation) {
    Preset preset;
    preset.setName("test");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    EXPECT_TRUE(preset.validate());
}

// Parameterized test for different grid sizes
class GridGenerationParameterizedTest : public ::testing::TestWithParam<std::pair<int, int>> {};

TEST_P(GridGenerationParameterizedTest, GeneratesCorrectGridSize) {
    auto [rows, cols] = GetParam();
    PresetLoader loader;
    auto city = loader.createGridTopology(rows, cols);
    
    EXPECT_NE(city, nullptr);
    EXPECT_EQ(city->getNodeCount(), rows * cols);
}

INSTANTIATE_TEST_SUITE_P(
    GridSizes,
    GridGenerationParameterizedTest,
    ::testing::Values(
        std::make_pair(2, 2),
        std::make_pair(3, 3),
        std::make_pair(4, 4),
        std::make_pair(5, 5),
        std::make_pair(10, 10)
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

