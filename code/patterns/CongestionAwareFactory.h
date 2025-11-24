// code/patterns/CongestionAwareFactory.h
#pragma once

#include "IPolicyFactory.h"

/**
 * Abstract Factory Pattern: Concrete Factory - Congestion Aware Policy
 */
class CongestionAwareFactory : public IPolicyFactory {
public:
    CongestionAwareFactory() = default;
    ~CongestionAwareFactory() override = default;
    
    std::unique_ptr<IRoutePolicy> createPolicy() override;
    std::string getPolicyName() const override;
    std::string getDescription() const override;
    PolicyType getPolicyType() const override;
};

