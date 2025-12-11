// code/adapters/PresetLoader.cpp
#include "PresetLoader.h"
#include "JsonReader.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"
#include "../core/Agent.h"
#include "../core/Preset.h"
#include <random>
#include <unordered_set>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>

// Simple JSON parsing helper functions
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
    
    std::string extractStringValue(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";
        
        pos = json.find(':', pos);
        if (pos == std::string::npos) return "";
        
        pos = json.find('"', pos);
        if (pos == std::string::npos) return "";
        
        size_t start = pos + 1;
        size_t end = json.find('"', start);
        if (end == std::string::npos) return "";
        
        return json.substr(start, end - start);
    }
    
    int extractIntValue(const std::string& json, const std::string& key, int defaultValue = 0) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return defaultValue;
        
        pos = json.find(':', pos);
        if (pos == std::string::npos) return defaultValue;
        
        // Skip whitespace
        pos++;
        while (pos < json.size() && std::isspace(json[pos])) pos++;
        
        // Read number
        std::string numStr;
        while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-')) {
            numStr += json[pos];
            pos++;
        }
        
        if (numStr.empty()) return defaultValue;
        return std::stoi(numStr);
    }
    
    std::vector<std::pair<int, int>> extractBlockedEdges(const std::string& json) {
        std::vector<std::pair<int, int>> blocked;
        
        std::string searchKey = "\"blocked\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return blocked;
        
        pos = json.find('[', pos);
        if (pos == std::string::npos) return blocked;
        
        size_t end = json.find(']', pos);
        if (end == std::string::npos) return blocked;
        
        std::string arrayContent = json.substr(pos + 1, end - pos - 1);
        
        // Parse array of pairs like [[0,1], [2,3]]
        size_t innerStart = 0;
        while ((innerStart = arrayContent.find('[', innerStart)) != std::string::npos) {
            size_t innerEnd = arrayContent.find(']', innerStart);
            if (innerEnd == std::string::npos) break;
            
            std::string pairStr = arrayContent.substr(innerStart + 1, innerEnd - innerStart - 1);
            
            // Extract two numbers
            std::istringstream iss(pairStr);
            int from, to;
            char comma;
            if (iss >> from >> comma >> to) {
                blocked.push_back({from, to});
            }
            
            innerStart = innerEnd + 1;
        }
        
        return blocked;
    }
}

Preset PresetLoader::loadFromJson(const std::string& path) {
    JsonReader reader;
    std::string json;
    
    try {
        json = reader.read(path);
    } catch (const std::exception& e) {
        // Re-throw so caller can try different paths
        throw std::runtime_error("Failed to read file: " + path + " - " + e.what());
    }
    
    if (json.empty()) {
        throw std::runtime_error("File is empty: " + path);
    }
    
    Preset preset;
    
    // Parse JSON fields
    std::string name = extractStringValue(json, "name");
    preset.setName(name.empty() ? "unnamed" : name);
    
    int rows = extractIntValue(json, "rows", 5);
    preset.setRows(rows);
    
    int cols = extractIntValue(json, "cols", 5);
    preset.setCols(cols);
    
    int agentCount = extractIntValue(json, "agentCount", 10);
    preset.setAgentCount(agentCount);
    
    int tickMs = extractIntValue(json, "tickMs", 100);
    preset.setTickMs(tickMs);
    
    // Parse policy
    std::string policyStr = extractStringValue(json, "policy");
    if (policyStr == "CONGESTION_AWARE" || policyStr == "congestion_aware") {
        preset.setPolicy(PolicyType::CONGESTION_AWARE);
    } else {
        preset.setPolicy(PolicyType::SHORTEST_PATH);
    }
    
    // Parse blocked edges
    std::vector<std::pair<int, int>> blocked = extractBlockedEdges(json);
    std::vector<std::pair<NodeId, NodeId>> blockedEdges;
    for (const auto& pair : blocked) {
        blockedEdges.push_back({static_cast<NodeId>(pair.first), static_cast<NodeId>(pair.second)});
    }
    preset.setBlockedEdges(blockedEdges);
    
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
    
    // Scale capacity based on grid size
    // Smaller grids = lower capacity (traffic shows more)
    // Larger grids = higher capacity (more room for agents)
    int gridSize = rows * cols;
    int capacity;
    if (gridSize <= 25) {         // 5x5 or smaller
        capacity = 2;
    } else if (gridSize <= 64) {  // 6x6 to 8x8
        capacity = 3;
    } else if (gridSize <= 144) { // 9x9 to 12x12
        capacity = 4;
    } else {                      // 13x13 and larger
        capacity = 5;
    }
    
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
            
            // Create edge with scaled capacity
            Edge edge(edgeId, from, to, 1.0, capacity);
            city->addEdge(edge);
            edgeId++;
            
            // Create reverse edge (bidirectional)
            Edge reverseEdge(edgeId, to, from, 1.0, capacity);
            city->addEdge(reverseEdge);
            edgeId++;
        }
    }
    
    // Vertical edges (top-bottom)
    for (int row = 0; row < rows - 1; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId from = row * cols + col;
            NodeId to = (row + 1) * cols + col;
            
            Edge edge(edgeId, from, to, 1.0, capacity);
            city->addEdge(edge);
            edgeId++;
            
            // Create reverse edge (bidirectional)
            Edge reverseEdge(edgeId, to, from, 1.0, capacity);
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
