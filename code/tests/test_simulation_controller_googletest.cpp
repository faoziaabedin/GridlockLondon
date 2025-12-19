// code/tests/test_simulation_controller_googletest.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "../core/SimulationController.h"
#include "../core/Preset.h"
#include "../core/Metrics.h"
#include "mocks/MockCity.h"

/**
 * Test Suite 4: SimulationController (10+ tests)
 * Tests state management, policy switching, reset, and multi-agent coordination.
 */

class SimulationControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        controller = std::make_unique<SimulationController>();
    }
    
    void TearDown() override {
        controller.reset();
    }
    
    std::unique_ptr<SimulationController> controller;
    
    Preset createTestPreset() {
        Preset preset;
        preset.setName("test");
        preset.setRows(3);
        preset.setCols(3);
        preset.setAgentCount(5);
        preset.setTickMs(100);
        preset.setPolicy(PolicyType::SHORTEST_PATH);
        return preset;
    }
};

// Test 1: Controller creation
TEST_F(SimulationControllerTest, ControllerCreation) {
    EXPECT_NE(controller.get(), nullptr);
    // City is null until loadPreset is called
    EXPECT_EQ(controller->getCity(), nullptr);
    EXPECT_NE(controller->getMetrics(), nullptr);
}

// Test 2: Load preset
TEST_F(SimulationControllerTest, LoadPreset) {
    Preset preset = createTestPreset();
    
    EXPECT_NO_THROW({
        controller->loadPreset(preset);
    });
    
    EXPECT_NE(controller->getCity(), nullptr);
    EXPECT_GT(controller->getAgents().size(), 0);
}

// Test 3: Start simulation
TEST_F(SimulationControllerTest, StartSimulation) {
    Preset preset = createTestPreset();
    controller->loadPreset(preset);
    
    EXPECT_NO_THROW({
        controller->start();
    });
}

// Test 4: Pause simulation
TEST_F(SimulationControllerTest, PauseSimulation) {
    Preset preset = createTestPreset();
    controller->loadPreset(preset);
    controller->start();
    
    EXPECT_NO_THROW({
        controller->pause();
    });
}

// Test 5: Reset simulation
TEST_F(SimulationControllerTest, ResetSimulation) {
    Preset preset = createTestPreset();
    controller->loadPreset(preset);
    controller->start();
    controller->tick();
    controller->tick();
    
    EXPECT_NO_THROW({
        controller->reset();
    });
    
    // After reset, should be back to initial state
    EXPECT_NE(controller->getCity(), nullptr);
}

// Test 6: Policy switching
TEST_F(SimulationControllerTest, PolicySwitching) {
    Preset preset = createTestPreset();
    controller->loadPreset(preset);
    
    EXPECT_EQ(controller->getPolicy(), PolicyType::SHORTEST_PATH);
    
    controller->setPolicy(PolicyType::CONGESTION_AWARE);
    EXPECT_EQ(controller->getPolicy(), PolicyType::CONGESTION_AWARE);
    
    controller->setPolicy(PolicyType::SHORTEST_PATH);
    EXPECT_EQ(controller->getPolicy(), PolicyType::SHORTEST_PATH);
}

// Test 7: Multi-agent coordination
TEST_F(SimulationControllerTest, MultiAgentCoordination) {
    Preset preset = createTestPreset();
    preset.setAgentCount(10);
    controller->loadPreset(preset);
    
    EXPECT_EQ(controller->getAgents().size(), 10);
    
    // All agents should have valid routes
    for (auto* agent : controller->getAgents()) {
        EXPECT_NE(agent, nullptr);
    }
}

// Test 8: Tick advances simulation
TEST_F(SimulationControllerTest, TickAdvancesSimulation) {
    Preset preset = createTestPreset();
    controller->loadPreset(preset);
    
    int initialTick = controller->getMetrics()->getCurrentTick();
    controller->tick();
    int newTick = controller->getMetrics()->getCurrentTick();
    
    EXPECT_GT(newTick, initialTick);
}

// Test 9: State management - start/pause/reset cycle
TEST_F(SimulationControllerTest, StartPauseResetCycle) {
    Preset preset = createTestPreset();
    controller->loadPreset(preset);
    
    controller->start();
    controller->pause();
    controller->reset();
    controller->start();
    
    // Should handle multiple state transitions
    EXPECT_NE(controller->getCity(), nullptr);
}

// Test 10: Metrics update during simulation
TEST_F(SimulationControllerTest, MetricsUpdateDuringSimulation) {
    Preset preset = createTestPreset();
    controller->loadPreset(preset);
    
    int initialThroughput = controller->getMetrics()->totalThroughput();
    
    // Run several ticks
    for (int i = 0; i < 10; ++i) {
        controller->tick();
    }
    
    // Metrics should have updated (throughput may or may not increase depending on agents arriving)
    EXPECT_GE(controller->getMetrics()->getCurrentTick(), 10);
}

// Test 11: Singleton access
TEST_F(SimulationControllerTest, SingletonAccess) {
    SimulationController& instance1 = SimulationController::getInstance();
    SimulationController& instance2 = SimulationController::getInstance();
    
    EXPECT_EQ(&instance1, &instance2);
}

// Test 12: Large preset handling
TEST_F(SimulationControllerTest, LargePresetHandling) {
    Preset preset;
    preset.setName("large");
    preset.setRows(10);
    preset.setCols(10);
    preset.setAgentCount(50);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    
    EXPECT_NO_THROW({
        controller->loadPreset(preset);
    });
    
    EXPECT_EQ(controller->getAgents().size(), 50);
}

// Parameterized test for different policy types
class SimulationControllerPolicyTest : public ::testing::TestWithParam<PolicyType> {};

TEST_P(SimulationControllerPolicyTest, WorksWithDifferentPolicies) {
    SimulationController controller;
    Preset preset;
    preset.setName("test");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(GetParam());
    
    EXPECT_NO_THROW({
        controller.loadPreset(preset);
        controller.setPolicy(GetParam());
        EXPECT_EQ(controller.getPolicy(), GetParam());
    });
}

INSTANTIATE_TEST_SUITE_P(
    PolicyTypes,
    SimulationControllerPolicyTest,
    ::testing::Values(
        PolicyType::SHORTEST_PATH,
        PolicyType::CONGESTION_AWARE
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

