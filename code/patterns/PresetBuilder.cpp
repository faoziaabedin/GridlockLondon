// code/patterns/PresetBuilder.cpp
#include "PresetBuilder.h"
#include "../core/Preset.h"
#include <stdexcept>

PresetBuilder::PresetBuilder()
    : name(""),
      rows(0),
      cols(0),
      agentCount(0),
      policy(PolicyType::SHORTEST_PATH),
      tickMs(100) {
}

PresetBuilder& PresetBuilder::setName(const std::string& name) {
    this->name = name;
    return *this;
}

PresetBuilder& PresetBuilder::setGridSize(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    return *this;
}

PresetBuilder& PresetBuilder::setAgentCount(int count) {
    this->agentCount = count;
    return *this;
}

PresetBuilder& PresetBuilder::setPolicy(PolicyType type) {
    this->policy = type;
    return *this;
}

PresetBuilder& PresetBuilder::addBlockedEdge(NodeId from, NodeId to) {
    blockedEdges.push_back({from, to});
    return *this;
}

PresetBuilder& PresetBuilder::addBlockedEdges(const std::vector<std::pair<NodeId, NodeId>>& edges) {
    blockedEdges.insert(blockedEdges.end(), edges.begin(), edges.end());
    return *this;
}

PresetBuilder& PresetBuilder::setTickInterval(int ms) {
    this->tickMs = ms;
    return *this;
}

PresetBuilder& PresetBuilder::reset() {
    name = "";
    rows = 0;
    cols = 0;
    agentCount = 0;
    policy = PolicyType::SHORTEST_PATH;
    blockedEdges.clear();
    tickMs = 100;
    return *this;
}

Preset PresetBuilder::build() {
    Preset preset = createPreset();
    
    if (!preset.validate()) {
        throw std::runtime_error("Invalid preset configuration: validation failed");
    }
    
    return preset;
}

bool PresetBuilder::isValid() const {
    Preset preset = createPreset();
    return preset.validate();
}

Preset PresetBuilder::createPreset() const {
    Preset preset;
    preset.setName(name);
    preset.setRows(rows);
    preset.setCols(cols);
    preset.setAgentCount(agentCount);
    preset.setPolicy(policy);
    preset.setBlockedEdges(blockedEdges);
    preset.setTickMs(tickMs);
    return preset;
}

