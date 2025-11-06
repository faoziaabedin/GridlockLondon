// code/core/SimulationController.h
#pragma once

#include <memory>
#include <vector>
#include "Preset.h"
#include "IRoutePolicy.h"

class City;
class RoutePlanner;
class Metrics;
class Agent;

/**
 * SimulationController orchestrates the entire simulation loop.
 * Manages city, agents, routing, and metrics.
 */
class SimulationController {
public:
    SimulationController();
    ~SimulationController();

    // Optional singleton access (for UML compatibility)
    static SimulationController& getInstance();

    // Preset loading
    void loadPreset(const Preset& preset);

    // Simulation control
    void start();
    void pause();
    void reset();
    void tick();

    // Policy management
    void setPolicy(PolicyType policy);

    // Getters
    City* getCity() const;
    std::vector<Agent*>& getAgents();
    Metrics* getMetrics() const;

private:
    // Helper methods
    void buildGridCity(int rows, int cols, const std::vector<std::pair<NodeId, NodeId>>& blockedEdges);
    void createAgents(int count, int totalNodes);
    std::unique_ptr<IRoutePolicy> createPolicy(PolicyType policy);
    void saveInitialState();  // For reset functionality

    // Data members (as per requirements)
    std::unique_ptr<City> city;
    std::unique_ptr<RoutePlanner> planner;
    std::unique_ptr<Metrics> metrics;
    std::vector<std::unique_ptr<Agent>> agents;
    bool running = false;
    int tickMs = 100;

    // For reset functionality - store initial agent states
    std::vector<std::pair<NodeId, NodeId>> initialAgentRoutes;
    std::unique_ptr<IRoutePolicy> currentPolicy;
    
    // Helper for getAgents() - return vector of raw pointers
    mutable std::vector<Agent*> agentsPtrs;
    
    // Singleton instance (optional)
    static std::unique_ptr<SimulationController> instance_;
};

