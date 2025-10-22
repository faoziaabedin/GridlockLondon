// code/core/RoutePlanner.h
#pragma once
#include "IRoutePolicy.h"
#include "Types.h"
#include <deque>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>

// Forward declarations
class City;
class Agent;

/**
 * RoutePlanner provides pathfinding functionality using Dijkstra's algorithm.
 * Acts as a façade that can work with different routing policies.
 */
class RoutePlanner {
public:
    /**
     * Constructor that takes a routing policy.
     * @param p Pointer to the routing policy to use
     */
    explicit RoutePlanner(IRoutePolicy* p);
    
    /**
     * Switch to a different routing policy.
     * @param p Pointer to the new routing policy
     */
    void setPolicy(IRoutePolicy* p);
    
    /**
     * Compute the optimal path for an agent using the current policy.
     * @param city Reference to the city containing the network
     * @param agent Reference to the agent needing a path
     * @return Deque of EdgeIds representing the path, empty if no path exists
     */
    std::deque<EdgeId> computePath(City& city, Agent& agent);

private:
    /**
     * Dijkstra's algorithm implementation.
     * Finds the shortest path from start to goal using the current policy's edge costs.
     * @param city Reference to the city containing the network
     * @param start Starting node ID
     * @param goal Destination node ID
     * @return Deque of EdgeIds representing the path, empty if no path exists
     */
    std::deque<EdgeId> dijkstra(City& city, NodeId start, NodeId goal);
    
    /**
     * Reconstruct the path from the predecessor map.
     * @param predecessors Map of node to previous node
     * @param edgeMap Map of (from, to) -> edgeId
     * @param start Starting node ID
     * @param goal Destination node ID
     * @return Deque of EdgeIds representing the path
     */
    std::deque<EdgeId> reconstructPath(const std::unordered_map<NodeId, NodeId>& predecessors,
                                     const std::unordered_map<std::pair<NodeId, NodeId>, EdgeId, 
                                                             std::hash<std::pair<NodeId, NodeId>>>& edgeMap,
                                     NodeId start, NodeId goal);
    
    /**
     * Current routing policy.
     */
    IRoutePolicy* policy{nullptr};
};