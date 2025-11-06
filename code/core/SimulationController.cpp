// code/core/SimulationController.cpp
#include "SimulationController.h"
#include "City.h"
#include "RoutePlanner.h"
#include "Metrics.h"
#include "Agent.h"
#include "Node.h"
#include "Edge.h"
#include "ShortestPathPolicy.h"
#include "CongestionAwarePolicy.h"
#include "../adapters/PresetLoader.h"
#include <random>
#include <algorithm>
#include <unordered_set>

// Singleton instance
std::unique_ptr<SimulationController> SimulationController::instance_ = nullptr;

SimulationController::SimulationController()
    : city(nullptr),
      planner(nullptr),
      metrics(std::make_unique<Metrics>()),
      running(false),
      tickMs(100) {
}

SimulationController::~SimulationController() = default;

SimulationController& SimulationController::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<SimulationController>(new SimulationController());
    }
    return *instance_;
}

void SimulationController::loadPreset(const Preset& preset) {
    if (!preset.validate()) {
        throw std::runtime_error("Invalid preset configuration");
    }

    // Reset any existing state
    reset();

    // Store preset settings
    tickMs = preset.getTickMs();

    // Use PresetLoader to build city
    PresetLoader loader;
    city = loader.buildCity(preset);

    // Use PresetLoader to spawn agents
    agents = loader.spawnAgents(preset, *city);
    
    // Update agentsPtrs
    agentsPtrs.clear();
    for (auto& agent : agents) {
        agentsPtrs.push_back(agent.get());
    }

    // Create and set the routing policy
    currentPolicy = createPolicy(preset.getPolicy());
    planner = std::make_unique<RoutePlanner>(currentPolicy.get());

    // Save initial state for reset
    saveInitialState();
}

void SimulationController::buildGridCity(int rows, int cols, 
                                         const std::vector<std::pair<NodeId, NodeId>>& blockedEdges) {
    // This method is kept for backward compatibility but now uses PresetLoader
    PresetLoader loader;
    city = loader.createGridTopology(rows, cols);
    loader.applyBlockedEdges(*city, blockedEdges);
}

void SimulationController::createAgents(int count, int totalNodes) {
    agents.clear();
    agentsPtrs.clear();
    
    // Use a simple deterministic pattern for agent origins and destinations
    // This ensures reproducibility while still having varied routes
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<NodeId> nodeDist(0, totalNodes - 1);
    
    for (int i = 0; i < count; ++i) {
        NodeId origin = nodeDist(rng);
        NodeId destination = nodeDist(rng);
        
        // Ensure origin != destination
        while (origin == destination) {
            destination = nodeDist(rng);
        }
        
        agents.push_back(std::make_unique<Agent>(i, origin, destination));
        agentsPtrs.push_back(agents.back().get());
    }
}

std::unique_ptr<IRoutePolicy> SimulationController::createPolicy(PolicyType policy) {
    switch (policy) {
        case PolicyType::SHORTEST_PATH:
            return std::make_unique<ShortestPathPolicy>();
        case PolicyType::CONGESTION_AWARE:
            return std::make_unique<CongestionAwarePolicy>();
        default:
            return std::make_unique<ShortestPathPolicy>();
    }
}

void SimulationController::saveInitialState() {
    initialAgentRoutes.clear();
    for (const auto& agent : agents) {
        initialAgentRoutes.push_back({agent->getOrigin(), agent->getDestination()});
    }
}

void SimulationController::start() {
    running = true;
}

void SimulationController::pause() {
    running = false;
}

void SimulationController::reset() {
    running = false;
    
    // Reset metrics
    if (metrics) {
        metrics->reset();
    }
    
    // Reset agents to initial state
    if (!initialAgentRoutes.empty() && agents.size() == initialAgentRoutes.size()) {
        // Recreate agents with original origin/destination
        for (size_t i = 0; i < agents.size(); ++i) {
            NodeId origin = initialAgentRoutes[i].first;
            NodeId destination = initialAgentRoutes[i].second;
            agents[i] = std::make_unique<Agent>(static_cast<int>(i), origin, destination);
            agentsPtrs[i] = agents[i].get();
        }
    }
    
    // Reset city occupancy
    if (city) {
        // Reset all edge occupancies to 0
        // We need to iterate through all edges - this is a simplified approach
        // In a full implementation, City would have a method to reset all occupancies
    }
}

void SimulationController::tick() {
    if (!city || !planner || !metrics) {
        return;
    }

    // Update metrics for this tick
    metrics->tick();

    // Process each agent
    for (auto& agent : agents) {
        // Skip if agent has already arrived
        if (agent->hasArrived()) {
            continue;
        }

        // Check if agent needs a route or should reroute
        bool needsReroute = false;
        
        // If agent needs initial route
        if (agent->needsRoute()) {
            // Compute path from origin to destination
            std::deque<EdgeId> path = planner->computePath(*city, *agent);
            if (!path.empty()) {
                agent->setPath(path);
            }
        } else {
            // Check if agent should reroute based on policy
            // Agents reroute when they reach a node (not on an edge)
            // and the policy says to reroute
            if (!agent->getCurrentEdge().has_value() && currentPolicy) {
                // Agent is at a node, check if it should reroute
                if (currentPolicy->shouldRerouteOnNode(*agent)) {
                    needsReroute = true;
                }
            }
        }

        // If rerouting is needed, compute new path from current node
        if (needsReroute) {
            std::deque<EdgeId> newPath = planner->computePath(*city, *agent);
            if (!newPath.empty()) {
                agent->setPath(newPath);
            }
        }

        // Record departure if this is the first tick for this agent
        // (in a more complete implementation, we'd track this better)
        
        // Move the agent one step
        agent->step(*city);

        // Check if agent just arrived
        if (agent->hasArrived()) {
            // Record arrival in metrics
            int travelTime = agent->getTravelTime();
            metrics->recordArrival(*agent, travelTime);
        }
        
        // Track max edge load by checking agent's current edge
        if (agent->getCurrentEdge().has_value()) {
            EdgeId edgeId = agent->getCurrentEdge().value();
            int load = city->occupancy(edgeId);
            metrics->updateMaxEdgeLoad(load);
        }
    }

    // Update metrics with current city state
    metrics->snapshotEdgeLoads(*city);
}

void SimulationController::setPolicy(PolicyType policy) {
    currentPolicy = createPolicy(policy);
    if (planner) {
        planner->setPolicy(currentPolicy.get());
    }
}

City* SimulationController::getCity() const {
    return city.get();
}

std::vector<Agent*>& SimulationController::getAgents() {
    // Update agentsPtrs to match current agents
    agentsPtrs.clear();
    for (auto& agent : agents) {
        agentsPtrs.push_back(agent.get());
    }
    return agentsPtrs;
}

Metrics* SimulationController::getMetrics() const {
    return metrics.get();
}
