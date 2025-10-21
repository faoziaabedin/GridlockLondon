// code/core/Preset.h
#pragma once
#include "Types.h"
#include <string>
#include <vector>
#include <utility>

enum class PolicyType {
    SHORTEST_PATH,
    CONGESTION_AWARE
};

class Preset {
public:
    Preset();
    
    // Validation
    bool validate() const;
    
    // Getters
    const std::string& getName() const;
    int getRows() const;
    int getCols() const;
    const std::vector<std::pair<NodeId, NodeId>>& getBlockedEdges() const;
    int getAgentCount() const;
    int getTickMs() const;
    PolicyType getPolicy() const;
    
    // Setters
    void setName(const std::string& name);
    void setRows(int rows);
    void setCols(int cols);
    void setBlockedEdges(const std::vector<std::pair<NodeId, NodeId>>& blocked);
    void setAgentCount(int count);
    void setTickMs(int ms);
    void setPolicy(PolicyType policy);
    
private:
    std::string name;
    int rows;
    int cols;
    std::vector<std::pair<NodeId, NodeId>> blockedEdges;
    int agentCount;
    int tickMs;
    PolicyType policy;
};