// code/core/City.cpp
#include "City.h"
#include <stdexcept>
#include <algorithm>
#include <string>

void City::addNode(const Node& node) {
    nodes.push_back(node);
    // Initialize adjacency list for this node
    adjacency[node.getId()] = std::vector<EdgeId>();
}

void City::addEdge(const Edge& edge) {
    edges.push_back(edge);
    // Initialize occupancy to 0
    occ[edge.getId()] = 0;
    // Add edge to adjacency list of the 'from' node
    adjacency[edge.getFrom()].push_back(edge.getId());
}

Node& City::getNode(NodeId id) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
                          [id](const Node& n) { return n.getId() == id; });
    if (it == nodes.end()) {
        throw std::runtime_error("Node not found: " + std::to_string(id));
    }
    return *it;
}

Edge& City::getEdge(EdgeId id) {
    auto it = std::find_if(edges.begin(), edges.end(),
                          [id](const Edge& e) { return e.getId() == id; });
    if (it == edges.end()) {
        throw std::runtime_error("Edge not found: " + std::to_string(id));
    }
    return *it;
}

const Node& City::getNode(NodeId id) const {
    auto it = std::find_if(nodes.begin(), nodes.end(),
                          [id](const Node& n) { return n.getId() == id; });
    if (it == nodes.end()) {
        throw std::runtime_error("Node not found: " + std::to_string(id));
    }
    return *it;
}

const Edge& City::getEdge(EdgeId id) const {
    auto it = std::find_if(edges.begin(), edges.end(),
                          [id](const Edge& e) { return e.getId() == id; });
    if (it == edges.end()) {
        throw std::runtime_error("Edge not found: " + std::to_string(id));
    }
    return *it;
}

std::vector<EdgeId> City::neighbors(NodeId nodeId) const {
    auto it = adjacency.find(nodeId);
    if (it == adjacency.end()) {
        return std::vector<EdgeId>();  // Return empty vector if node has no neighbors
    }
    return it->second;
}

int City::edgeCapacity(EdgeId edgeId) const {
    return getEdge(edgeId).getCapacity();
}

double City::edgeLength(EdgeId edgeId) const {
    return getEdge(edgeId).getLength();
}

int City::occupancy(EdgeId edgeId) const {
    auto it = occ.find(edgeId);
    if (it == occ.end()) {
        return 0;  // Default to 0 if not found
    }
    return it->second;
}

void City::setOccupancy(EdgeId edgeId, int occupancy) {
    int capacity = edgeCapacity(edgeId);
    // Clamp occupancy to valid range [0, capacity]
    if (occupancy < 0) {
        occupancy = 0;
    } else if (occupancy > capacity) {
        occupancy = capacity;
    }
    occ[edgeId] = occupancy;
}

void City::incrementOccupancy(EdgeId edgeId) {
    int current = occupancy(edgeId);
    int capacity = edgeCapacity(edgeId);
    // Only increment if below capacity
    if (current < capacity) {
        occ[edgeId] = current + 1;
    }
}

void City::decrementOccupancy(EdgeId edgeId) {
    int current = occupancy(edgeId);
    // Only decrement if above 0
    if (current > 0) {
        occ[edgeId] = current - 1;
    }
}

int City::getNodeCount() const {
    return static_cast<int>(nodes.size());
}

int City::getEdgeCount() const {
    return static_cast<int>(edges.size());
}

NodeId City::getNodeIdByIndex(int index) const {
    if (index < 0 || index >= static_cast<int>(nodes.size())) {
        throw std::runtime_error("Node index out of range: " + std::to_string(index));
    }
    return nodes[index].getId();
}

EdgeId City::getEdgeIdByIndex(int index) const {
    if (index < 0 || index >= static_cast<int>(edges.size())) {
        throw std::runtime_error("Edge index out of range: " + std::to_string(index));
    }
    return edges[index].getId();
}