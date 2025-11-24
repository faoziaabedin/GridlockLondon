// code/patterns/PolicyRegistry.h
#pragma once

#include "IPolicyFactory.h"
#include <memory>
#include <string>
#include <map>
#include <vector>

/**
 * Abstract Factory Pattern: Policy Registry
 * 
 * Combines Abstract Factory with Registry pattern for dynamic policy selection.
 * 
 * Benefits:
 * - Extensible: Add new policies without modifying existing code
 * - Dynamic: Select policies by name at runtime
 * - Discoverable: Query available policies
 * - Type-safe: Returns proper factory interface
 */
class PolicyRegistry {
public:
    /**
     * Get the singleton instance of the registry.
     */
    static PolicyRegistry& getInstance();
    
    /**
     * Register a policy factory.
     * @param name Unique name identifier for the policy
     * @param factory Unique pointer to the factory (registry takes ownership)
     */
    void registerFactory(const std::string& name, std::unique_ptr<IPolicyFactory> factory);
    
    /**
     * Create a policy by name.
     * @param name Policy name (must be registered)
     * @return Unique pointer to policy instance
     * @throws std::runtime_error if policy not found
     */
    std::unique_ptr<IRoutePolicy> createPolicy(const std::string& name);
    
    /**
     * Create a policy by PolicyType enum.
     * @param type Policy type
     * @return Unique pointer to policy instance
     * @throws std::runtime_error if policy not found
     */
    std::unique_ptr<IRoutePolicy> createPolicy(PolicyType type);
    
    /**
     * Get list of available policy names.
     * @return Vector of policy names
     */
    std::vector<std::string> getAvailablePolicies() const;
    
    /**
     * Get policy metadata.
     * @param name Policy name
     * @return Policy name and description pair
     * @throws std::runtime_error if policy not found
     */
    std::pair<std::string, std::string> getPolicyInfo(const std::string& name) const;
    
    /**
     * Check if a policy is registered.
     * @param name Policy name
     * @return true if registered
     */
    bool isRegistered(const std::string& name) const;
    
    /**
     * Initialize with default policies (ShortestPath, CongestionAware).
     */
    void initializeDefaults();
    
private:
    PolicyRegistry() = default;
    ~PolicyRegistry() = default;
    PolicyRegistry(const PolicyRegistry&) = delete;
    PolicyRegistry& operator=(const PolicyRegistry&) = delete;
    
    std::map<std::string, std::unique_ptr<IPolicyFactory>> factories;
    std::map<PolicyType, std::string> typeToName;
    
    static PolicyRegistry* instance;
};

