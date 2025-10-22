// code/core/CongestionAwarePolicy.h
#pragma once
#include "IRoutePolicy.h"

/**
 * Route policy that considers both distance and traffic congestion.
 * This policy balances shortest path with congestion avoidance.
 * 
 * Cost formula: length + alpha * (occupancy / capacity)
 * - Higher occupancy relative to capacity increases cost
 * - Agents reroute at every node to adapt to changing conditions
 */
class CongestionAwarePolicy : public IRoutePolicy {
public:
    /**
     * Calculate edge cost considering both distance and congestion.
     * Formula: length + alpha * (occupancy / capacity)
     * @param city Reference to the city containing the edge
     * @param edgeId ID of the edge to evaluate
     * @return Cost considering distance and congestion
     */
    double edgeCost(const City& city, EdgeId edgeId) const override;
    
    /**
     * Determine if an agent should reroute when reaching a node.
     * CongestionAwarePolicy always reroutes to adapt to changing traffic.
     * @param agent Reference to the agent to evaluate
     * @return true (always reroute to adapt to congestion changes)
     */
    bool shouldRerouteOnNode(const Agent& agent) const override;

private:
    /**
     * Congestion weight factor.
     * Higher values make congestion have more impact on routing decisions.
     */
    double alpha{2.0};
};
