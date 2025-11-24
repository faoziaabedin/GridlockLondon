// code/patterns/ShortestPathFactory.h
#pragma once

#include "IPolicyFactory.h"

/**
 * Abstract Factory Pattern: Concrete Factory - Shortest Path Policy
 */
class ShortestPathFactory : public IPolicyFactory {
public:
    ShortestPathFactory() = default;
    ~ShortestPathFactory() override = default;
    
    std::unique_ptr<IRoutePolicy> createPolicy() override;
    std::string getPolicyName() const override;
    std::string getDescription() const override;
    PolicyType getPolicyType() const override;
};

