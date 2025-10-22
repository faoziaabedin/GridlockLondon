// code/core/IRoutePolicy.h
#pragma once
#include "Types.h"

// Forward declarations
class City;
class Agent;

/**
 * Abstract interface for route planning policies.
 * Defines the contract for different routing strategies.
 */
class IRoutePolicy {
public:
    /**
     * Calculate the cost of traversing an edge.
     * @param city Reference to the city containing the edge
     * @param edgeId ID of the edge to evaluate
     * @return Cost of traversing the edge
     */
    virtual double edgeCost(const City& city, EdgeId edgeId) const = 0;
    
    /**
     * Determine if an agent should reroute when reaching a node.
     * @param agent Reference to the agent to evaluate
     * @return true if the agent should reroute, false otherwise
     */
    virtual bool shouldRerouteOnNode(const Agent& agent) const = 0;
    
    /**
     * Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~IRoutePolicy() = default;
};