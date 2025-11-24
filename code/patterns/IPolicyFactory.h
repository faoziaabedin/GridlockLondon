// code/patterns/IPolicyFactory.h
#pragma once

#include <memory>
#include <string>
#include "../core/IRoutePolicy.h"
#include "../core/Preset.h"  // For PolicyType enum

/**
 * Abstract Factory Pattern: Policy Factory Interface
 * 
 * Defines the interface for creating routing policies.
 * Each concrete factory creates a specific policy type and provides
 * metadata about the policy (name, description).
 * 
 * This pattern allows us to:
 * - Extend with new policies without modifying existing code
 * - Provide metadata about policies
 * - Use a registry pattern for dynamic policy selection
 */
class IPolicyFactory {
public:
    virtual ~IPolicyFactory() = default;
    
    /**
     * Create a new instance of the policy.
     * @return Unique pointer to the policy instance
     */
    virtual std::unique_ptr<IRoutePolicy> createPolicy() = 0;
    
    /**
     * Get the name of the policy this factory creates.
     * @return Policy name (e.g., "ShortestPath", "CongestionAware")
     */
    virtual std::string getPolicyName() const = 0;
    
    /**
     * Get a description of the policy.
     * @return Human-readable description
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * Get the PolicyType enum value for this policy.
     * @return PolicyType enum value
     */
    virtual PolicyType getPolicyType() const = 0;
};

