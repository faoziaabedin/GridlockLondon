// code/patterns/PolicyRegistry.cpp
#include "PolicyRegistry.h"
#include "ShortestPathFactory.h"
#include "CongestionAwareFactory.h"
#include <stdexcept>

PolicyRegistry* PolicyRegistry::instance = nullptr;

PolicyRegistry& PolicyRegistry::getInstance() {
    if (!instance) {
        instance = new PolicyRegistry();
        instance->initializeDefaults();
    }
    return *instance;
}

void PolicyRegistry::registerFactory(const std::string& name, std::unique_ptr<IPolicyFactory> factory) {
    if (factories.find(name) != factories.end()) {
        throw std::runtime_error("Policy factory already registered: " + name);
    }
    
    PolicyType type = factory->getPolicyType();
    factories[name] = std::move(factory);
    typeToName[type] = name;
}

std::unique_ptr<IRoutePolicy> PolicyRegistry::createPolicy(const std::string& name) {
    auto it = factories.find(name);
    if (it == factories.end()) {
        throw std::runtime_error("Policy not found: " + name);
    }
    
    return it->second->createPolicy();
}

std::unique_ptr<IRoutePolicy> PolicyRegistry::createPolicy(PolicyType type) {
    auto it = typeToName.find(type);
    if (it == typeToName.end()) {
        throw std::runtime_error("Policy type not registered");
    }
    
    return createPolicy(it->second);
}

std::vector<std::string> PolicyRegistry::getAvailablePolicies() const {
    std::vector<std::string> names;
    for (const auto& pair : factories) {
        names.push_back(pair.first);
    }
    return names;
}

std::pair<std::string, std::string> PolicyRegistry::getPolicyInfo(const std::string& name) const {
    auto it = factories.find(name);
    if (it == factories.end()) {
        throw std::runtime_error("Policy not found: " + name);
    }
    
    return {it->second->getPolicyName(), it->second->getDescription()};
}

bool PolicyRegistry::isRegistered(const std::string& name) const {
    return factories.find(name) != factories.end();
}

void PolicyRegistry::initializeDefaults() {
    // Register default policies
    registerFactory("ShortestPath", std::make_unique<ShortestPathFactory>());
    registerFactory("CongestionAware", std::make_unique<CongestionAwareFactory>());
}

