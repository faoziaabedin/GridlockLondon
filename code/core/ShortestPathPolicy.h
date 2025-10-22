// code/core/ShortestPathPolicy.h
#pragma once
#include "IRoutePolicy.h"

/**
 * Route policy that uses shortest path based on edge length.
 * This policy minimizes the total distance traveled.
 */
class ShortestPathPolicy : public IRoutePolicy {
public:
    /**
     * Calculate edge cost as the length of the edge.
     * @param city Reference to the city containing the edge
     * @param edgeId ID of the edge to evaluate
     * @return Length of the edge as the cost
     */
    double edgeCost(const City& city, EdgeId edgeId) const override;
    
    /**
     * Determine if an agent should reroute when reaching a node.
     * Returns false unless the path is empty (agent needs initial route).
     * @param agent Reference to the agent to evaluate
     * @return false (agents don't reroute once they have a path)
     */
    bool shouldRerouteOnNode(const Agent& agent) const override;
};