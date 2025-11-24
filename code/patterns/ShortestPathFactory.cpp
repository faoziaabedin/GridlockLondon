// code/patterns/ShortestPathFactory.cpp
#include "ShortestPathFactory.h"
#include "../core/ShortestPathPolicy.h"

std::unique_ptr<IRoutePolicy> ShortestPathFactory::createPolicy() {
    return std::make_unique<ShortestPathPolicy>();
}

std::string ShortestPathFactory::getPolicyName() const {
    return "ShortestPath";
}

std::string ShortestPathFactory::getDescription() const {
    return "Finds the shortest path based on edge length only. "
           "Agents do not reroute unless their path becomes invalid.";
}

PolicyType ShortestPathFactory::getPolicyType() const {
    return PolicyType::SHORTEST_PATH;
}

