// code/core/Preset.cpp
#include "Preset.h"

Preset::Preset()
    : name(""),
      rows(0),
      cols(0),
      agentCount(0),
      tickMs(100),
      policy(PolicyType::SHORTEST_PATH) {
}

bool Preset::validate() const {
    // Check for valid grid dimensions
    if (rows <= 0 || cols <= 0) {
        return false;
    }
    
    // Check for reasonable grid size (not too large)
    if (rows > 100 || cols > 100) {
        return false;
    }
    
    // Check agent count is non-negative
    if (agentCount < 0) {
        return false;
    }
    
    // Check tick interval is positive
    if (tickMs <= 0) {
        return false;
    }
    
    // Validate blocked edges reference valid nodes
    int totalNodes = rows * cols;
    for (const auto& edge : blockedEdges) {
        if (edge.first < 0 || edge.first >= totalNodes ||
            edge.second < 0 || edge.second >= totalNodes) {
            return false;
        }
    }
    
    return true;
}

const std::string& Preset::getName() const {
    return name;
}

int Preset::getRows() const {
    return rows;
}

int Preset::getCols() const {
    return cols;
}

const std::vector<std::pair<NodeId, NodeId>>& Preset::getBlockedEdges() const {
    return blockedEdges;
}

int Preset::getAgentCount() const {
    return agentCount;
}

int Preset::getTickMs() const {
    return tickMs;
}

PolicyType Preset::getPolicy() const {
    return policy;
}

void Preset::setName(const std::string& name) {
    this->name = name;
}

void Preset::setRows(int rows) {
    this->rows = rows;
}

void Preset::setCols(int cols) {
    this->cols = cols;
}

void Preset::setBlockedEdges(const std::vector<std::pair<NodeId, NodeId>>& blocked) {
    this->blockedEdges = blocked;
}

void Preset::setAgentCount(int count) {
    this->agentCount = count;
}

void Preset::setTickMs(int ms) {
    this->tickMs = ms;
}

void Preset::setPolicy(PolicyType policy) {
    this->policy = policy;
}