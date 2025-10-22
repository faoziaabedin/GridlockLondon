// code/core/RoutePlanner.cpp
#include "RoutePlanner.h"
#include "City.h"
#include "Agent.h"
#include <algorithm>
#include <climits>

RoutePlanner::RoutePlanner(IRoutePolicy* p) : policy(p) {
    // Constructor initializes the policy pointer
}

void RoutePlanner::setPolicy(IRoutePolicy* p) {
    policy = p;
}

std::deque<EdgeId> RoutePlanner::computePath(City& city, Agent& agent) {
    // Check if we have a valid policy
    if (!policy) {
        return std::deque<EdgeId>(); // Return empty path if no policy
    }
    
    // Get agent's origin and destination
    NodeId start = agent.getOrigin();
    NodeId goal = agent.getDestination();
    
    // If already at destination, return empty path
    if (start == goal) {
        return std::deque<EdgeId>();
    }
    
    // Use Dijkstra to find the path
    return dijkstra(city, start, goal);
}

std::deque<EdgeId> RoutePlanner::dijkstra(City& city, NodeId start, NodeId goal) {
    // Check if we have a valid policy
    if (!policy) {
        return std::deque<EdgeId>();
    }
    
    // Distance map: node -> shortest distance from start
    std::unordered_map<NodeId, double> distances;
    
    // Predecessor map: node -> previous node
    std::unordered_map<NodeId, NodeId> predecessors;
    
    // Edge map: (from, to) -> edgeId for path reconstruction
    std::unordered_map<std::pair<NodeId, NodeId>, EdgeId, 
                      std::hash<std::pair<NodeId, NodeId>>> edgeMap;
    
    // Priority queue for Dijkstra: (distance, node)
    std::priority_queue<std::pair<double, NodeId>, 
                       std::vector<std::pair<double, NodeId>>, 
                       std::greater<std::pair<double, NodeId>>> pq;
    
    // Initialize distances to infinity
    // We'll add nodes as we discover them
    const double INF = std::numeric_limits<double>::infinity();
    
    // Start with the starting node
    distances[start] = 0.0;
    pq.push({0.0, start});
    
    while (!pq.empty()) {
        // Get the node with minimum distance
        auto [currentDist, currentNode] = pq.top();
        pq.pop();
        
        // Skip if we've already found a better path to this node
        if (currentDist > distances[currentNode]) {
            continue;
        }
        
        // If we've reached the goal, we can stop
        if (currentNode == goal) {
            break;
        }
        
        // Explore all outgoing edges from current node
        std::vector<EdgeId> outgoingEdges = city.neighbors(currentNode);
        
        for (EdgeId edgeId : outgoingEdges) {
            // Get the edge
            const Edge& edge = city.getEdge(edgeId);
            NodeId neighbor = edge.getTo();
            
            // Skip if edge is blocked
            if (edge.isBlocked()) {
                continue;
            }
            
            // Calculate edge cost using the current policy
            double edgeCost = policy->edgeCost(city, edgeId);
            
            // Calculate new distance to neighbor
            double newDist = currentDist + edgeCost;
            
            // If we found a better path to the neighbor
            if (distances.find(neighbor) == distances.end() || newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                predecessors[neighbor] = currentNode;
                
                // Store the edge for path reconstruction
                edgeMap[{currentNode, neighbor}] = edgeId;
                
                pq.push({newDist, neighbor});
            }
        }
    }
    
    // Check if we found a path to the goal
    if (distances.find(goal) == distances.end()) {
        return std::deque<EdgeId>(); // No path exists
    }
    
    // Reconstruct the path
    return reconstructPath(predecessors, edgeMap, start, goal);
}

std::deque<EdgeId> RoutePlanner::reconstructPath(const std::unordered_map<NodeId, NodeId>& predecessors,
                                                const std::unordered_map<std::pair<NodeId, NodeId>, EdgeId, 
                                                                        std::hash<std::pair<NodeId, NodeId>>>& edgeMap,
                                                NodeId start, NodeId goal) {
    std::deque<EdgeId> path;
    
    // Start from the goal and work backwards
    NodeId current = goal;
    
    while (current != start) {
        // Find the previous node
        auto predIt = predecessors.find(current);
        if (predIt == predecessors.end()) {
            // This shouldn't happen if Dijkstra worked correctly
            return std::deque<EdgeId>();
        }
        
        NodeId previous = predIt->second;
        
        // Find the edge between previous and current
        auto edgeIt = edgeMap.find({previous, current});
        if (edgeIt == edgeMap.end()) {
            // This shouldn't happen if we stored edges correctly
            return std::deque<EdgeId>();
        }
        
        EdgeId edgeId = edgeIt->second;
        path.push_front(edgeId); // Add to front to maintain correct order
        
        // Move to the previous node
        current = previous;
    }
    
    return path;
}
