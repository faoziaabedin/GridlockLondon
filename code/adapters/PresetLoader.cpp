// code/adapters/PresetLoader.cpp
#include "PresetLoader.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"
#include "../core/Agent.h"
#include "../core/Preset.h"
#include <random>
#include <unordered_set>
#include <stdexcept>

Preset PresetLoader::loadFromJson(const std::string& path) {
    // Stub implementation for Deliverable 2
    // In a full implementation, this would parse JSON and populate a Preset
    // For now, return a default preset
    Preset preset;
    preset.setName("default");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setTickMs(100);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    return preset;
}

std::unique_ptr<City> PresetLoader::buildCity(const Preset& preset) {
    if (!preset.validate()) {
        throw std::runtime_error("Invalid preset configuration");
    }
    
    // Create grid topology
    auto city = createGridTopology(preset.getRows(), preset.getCols());
    
    // Apply blocked edges
    applyBlockedEdges(*city, preset.getBlockedEdges());
    
    return city;
}

std::vector<std::unique_ptr<Agent>> PresetLoader::spawnAgents(const Preset& preset, City& city) {
    std::vector<std::unique_ptr<Agent>> agents;
    
    int totalNodes = preset.getRows() * preset.getCols();
    int agentCount = preset.getAgentCount();
    
    if (totalNodes < 2) {
        throw std::runtime_error("City must have at least 2 nodes to spawn agents");
    }
    
    // Use deterministic random number generator for reproducibility
    // Seed can be changed for different agent distributions
    std::mt19937 rng(42);  // Fixed seed for reproducibility
    std::uniform_int_distribution<NodeId> nodeDist(0, totalNodes - 1);
    
    for (int i = 0; i < agentCount; ++i) {
        NodeId origin = nodeDist(rng);
        NodeId destination = nodeDist(rng);
        
        // Ensure origin != destination
        int attempts = 0;
        while (origin == destination && attempts < 100) {
            destination = nodeDist(rng);
            attempts++;
        }
        
        // If still equal after attempts, skip this agent or use a default destination
        if (origin == destination) {
            destination = (origin + 1) % totalNodes;
        }
        
        agents.push_back(std::make_unique<Agent>(i, origin, destination));
    }
    
    return agents;
}

std::unique_ptr<City> PresetLoader::createGridTopology(int rows, int cols) {
    auto city = std::make_unique<City>();
    
    // Create nodes
    // Node ID = row * cols + col
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId nodeId = row * cols + col;
            city->addNode(Node(nodeId, row, col));
        }
    }
    
    // Create edges: horizontal and vertical connections
    int edgeId = 0;
    
    // Horizontal edges (left-right)
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols - 1; ++col) {
            NodeId from = row * cols + col;
            NodeId to = row * cols + col + 1;
            
            // Create edge with default length 1.0 and capacity 10
            Edge edge(edgeId, from, to, 1.0, 10);
            city->addEdge(edge);
            edgeId++;
            
            // Create reverse edge (bidirectional)
            Edge reverseEdge(edgeId, to, from, 1.0, 10);
            city->addEdge(reverseEdge);
            edgeId++;
        }
    }
    
    // Vertical edges (top-bottom)
    for (int row = 0; row < rows - 1; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId from = row * cols + col;
            NodeId to = (row + 1) * cols + col;
            
            Edge edge(edgeId, from, to, 1.0, 10);
            city->addEdge(edge);
            edgeId++;
            
            // Create reverse edge (bidirectional)
            Edge reverseEdge(edgeId, to, from, 1.0, 10);
            city->addEdge(reverseEdge);
            edgeId++;
        }
    }
    
    return city;
}

void PresetLoader::applyBlockedEdges(City& city, const std::vector<std::pair<NodeId, NodeId>>& blockedEdges) {
    // Create a set of blocked edges for quick lookup
    // Use a simple hash: maxNodeId * from + to
    int maxNodeId = 1000;  // Reasonable upper bound
    std::unordered_set<long long> blockedSet;
    
    for (const auto& edge : blockedEdges) {
        long long key1 = static_cast<long long>(edge.first) * maxNodeId + edge.second;
        long long key2 = static_cast<long long>(edge.second) * maxNodeId + edge.first;
        blockedSet.insert(key1);
        blockedSet.insert(key2);  // Block both directions
    }
    
    // Find and block matching edges
    // Since City doesn't expose edge iteration directly, we'll need to find edges
    // by checking all possible node pairs. This is a simplified approach.
    // In a full implementation, City would provide edge iteration.
    
    // For now, we'll iterate through edges by trying to find them
    // This is not ideal but works for the current City interface
    for (const auto& blocked : blockedEdges) {
        NodeId from = blocked.first;
        NodeId to = blocked.second;
        
        // Try to find edges connecting these nodes
        // We'll check neighbors of 'from' node
        std::vector<EdgeId> neighbors = city.neighbors(from);
        for (EdgeId edgeId : neighbors) {
            try {
                Edge& edge = city.getEdge(edgeId);
                if (edge.getTo() == to) {
                    edge.setBlocked(true);
                }
            } catch (...) {
                // Edge not found, skip
            }
        }
        
        // Also check reverse direction
        neighbors = city.neighbors(to);
        for (EdgeId edgeId : neighbors) {
            try {
                Edge& edge = city.getEdge(edgeId);
                if (edge.getTo() == from) {
                    edge.setBlocked(true);
                }
            } catch (...) {
                // Edge not found, skip
            }
        }
    }
}
