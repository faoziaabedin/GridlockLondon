// code/core/City.h
#pragma once
#include <vector>
#include <unordered_map>
#include "Types.h"
#include "Node.h"
#include "Edge.h"

class City {
public:
    City() = default;
    
    // Add nodes and edges
    void addNode(const Node& node);
    void addEdge(const Edge& edge);
    
    // Get nodes and edges (returns references for modification)
    Node& getNode(NodeId id);
    Edge& getEdge(EdgeId id);
    
    // Get nodes and edges (const versions)
    const Node& getNode(NodeId id) const;
    const Edge& getEdge(EdgeId id) const;
    
    // Get neighbors (outgoing edges from a node)
    std::vector<EdgeId> neighbors(NodeId nodeId) const;
    
    // Edge properties
    int edgeCapacity(EdgeId edgeId) const;
    double edgeLength(EdgeId edgeId) const;
    
    // Occupancy management
    int occupancy(EdgeId edgeId) const;
    void setOccupancy(EdgeId edgeId, int occ);
    void incrementOccupancy(EdgeId edgeId);
    void decrementOccupancy(EdgeId edgeId);
    
private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::unordered_map<EdgeId, int> occ;  // Edge occupancy
    std::unordered_map<NodeId, std::vector<EdgeId>> adjacency;  // Node -> outgoing edges
};