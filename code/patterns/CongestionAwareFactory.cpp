// code/patterns/CongestionAwareFactory.cpp
#include "CongestionAwareFactory.h"
#include "../core/CongestionAwarePolicy.h"

std::unique_ptr<IRoutePolicy> CongestionAwareFactory::createPolicy() {
    return std::make_unique<CongestionAwarePolicy>();
}

std::string CongestionAwareFactory::getPolicyName() const {
    return "CongestionAware";
}

std::string CongestionAwareFactory::getDescription() const {
    return "Considers both edge length and traffic congestion. "
           "Cost = length + alpha * (occupancy / capacity). "
           "Agents reroute at every node to find less congested paths.";
}

PolicyType CongestionAwareFactory::getPolicyType() const {
    return PolicyType::CONGESTION_AWARE;
}

