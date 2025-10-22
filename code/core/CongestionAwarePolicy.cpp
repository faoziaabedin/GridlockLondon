// code/core/CongestionAwarePolicy.cpp
#include "CongestionAwarePolicy.h"
#include "City.h"
#include "Agent.h"

double CongestionAwarePolicy::edgeCost(const City& city, EdgeId edgeId) const {
    // Get basic edge properties
    double length = city.edgeLength(edgeId);
    int capacity = city.edgeCapacity(edgeId);
    int occupancy = city.occupancy(edgeId);
    
    // Calculate congestion factor: occupancy / capacity
    double congestionFactor = static_cast<double>(occupancy) / static_cast<double>(capacity);
    
    // Apply formula: length + alpha * (occupancy / capacity)
    return length + alpha * congestionFactor;
}

bool CongestionAwarePolicy::shouldRerouteOnNode(const Agent& agent) const {
    // CongestionAwarePolicy always reroutes to adapt to changing traffic conditions
    return true;
}