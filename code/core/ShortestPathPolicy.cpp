// code/core/ShortestPathPolicy.cpp
#include "ShortestPathPolicy.h"
#include "City.h"
#include "Agent.h"

double ShortestPathPolicy::edgeCost(const City& city, EdgeId edgeId) const {
    return city.edgeLength(edgeId);
}

bool ShortestPathPolicy::shouldRerouteOnNode(const Agent& agent) const {
    // Only reroute if the agent has no path (needs initial route)
    return agent.getPath().empty();
}