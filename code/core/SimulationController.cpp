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

    // Build the grid city
    buildGridCity(preset.getRows(), preset.getCols(), preset.getBlockedEdges());

    // Create agents
    int totalNodes = preset.getRows() * preset.getCols();
    createAgents(preset.getAgentCount(), totalNodes);

    // Create and set the routing policy
    currentPolicy = createPolicy(preset.getPolicy());
    planner = std::make_unique<RoutePlanner>(currentPolicy.get());

    // Save initial state for reset
    saveInitialState();
}

void SimulationController::buildGridCity(int rows, int cols, 
                                         const std::vector<std::pair<NodeId, NodeId>>& blockedEdges) {
    city = std::make_unique<City>();

    // Create nodes in grid pattern
    // Node IDs: 0 to (rows*cols-1), numbered row by row
    // Node at row i, col j has ID = i * cols + j
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId nodeId = row * cols + col;
            city->addNode(Node(nodeId, row, col));
        }
    }

    // Create a set of blocked edges for quick lookup
    // Use a simple hash function: combine two NodeIds into a single key
    // For simplicity, use maxNodeId * from + to as the key
    int maxNodeId = rows * cols;
    std::unordered_set<long long> blockedSet;
    for (const auto& edge : blockedEdges) {
        long long key1 = static_cast<long long>(edge.first) * maxNodeId + edge.second;
        long long key2 = static_cast<long long>(edge.second) * maxNodeId + edge.first;
        blockedSet.insert(key1);
        blockedSet.insert(key2);
    }

    // Create edges: horizontal and vertical connections
    int edgeId = 0;
    
    // Horizontal edges (left-right)
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols - 1; ++col) {
            NodeId from = row * cols + col;
            NodeId to = row * cols + col + 1;
            
            // Check if this edge is blocked
            long long edgeKey = static_cast<long long>(from) * (rows * cols) + to;
            bool isBlocked = blockedSet.find(edgeKey) != blockedSet.end();
            
            // Create edge with default length 1.0 and capacity 10
            Edge edge(edgeId, from, to, 1.0, 10);
            if (isBlocked) {
                edge.setBlocked(true);
            }
            city->addEdge(edge);
            edgeId++;
            
            // Create reverse edge (bidirectional)
            Edge reverseEdge(edgeId, to, from, 1.0, 10);
            long long reverseKey = static_cast<long long>(to) * (rows * cols) + from;
            if (blockedSet.find(reverseKey) != blockedSet.end()) {
                reverseEdge.setBlocked(true);
            }
            city->addEdge(reverseEdge);
            edgeId++;
        }
    }
    
    // Vertical edges (top-bottom)
    for (int row = 0; row < rows - 1; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId from = row * cols + col;
            NodeId to = (row + 1) * cols + col;
            
            // Check if this edge is blocked
            long long edgeKey = static_cast<long long>(from) * (rows * cols) + to;
            bool isBlocked = blockedSet.find(edgeKey) != blockedSet.end();
            
            Edge edge(edgeId, from, to, 1.0, 10);
            if (isBlocked) {
                edge.setBlocked(true);
            }
            city->addEdge(edge);
            edgeId++;
            
            // Create reverse edge (bidirectional)
            Edge reverseEdge(edgeId, to, from, 1.0, 10);
            long long reverseKey = static_cast<long long>(to) * (rows * cols) + from;
            if (blockedSet.find(reverseKey) != blockedSet.end()) {
                reverseEdge.setBlocked(true);
            }
            city->addEdge(reverseEdge);
            edgeId++;
        }
    }
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

        // If rerouting is needed, compute new path
        // Note: RoutePlanner uses getOrigin() which might not be correct for mid-journey rerouting
        // For now, we'll recompute from origin, but ideally this should use current node
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
