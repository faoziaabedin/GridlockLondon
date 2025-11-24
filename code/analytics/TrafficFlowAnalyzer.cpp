// code/analytics/TrafficFlowAnalyzer.cpp
#include "TrafficFlowAnalyzer.h"
#include "../core/City.h"
#include "../core/Agent.h"
#include <algorithm>
#include <cmath>

TrafficFlowAnalyzer::TrafficFlowAnalyzer() {
}

std::vector<TrafficFlowAnalyzer::Hotspot> TrafficFlowAnalyzer::detectHotspots(
    const City& city, double threshold) {
    
    std::vector<Hotspot> hotspots;
    
    int edgeCount = city.getEdgeCount();
    for (int i = 0; i < edgeCount; ++i) {
        EdgeId edgeId = city.getEdgeIdByIndex(i);
        
        int occupancy = city.occupancy(edgeId);
        int capacity = city.edgeCapacity(edgeId);
        
        if (capacity == 0) continue;
        
        double utilization = static_cast<double>(occupancy) / capacity;
        
        if (utilization >= threshold) {
            Hotspot hotspot;
            hotspot.edgeId = edgeId;
            hotspot.congestionLevel = calculateCongestionLevel(occupancy, capacity);
            hotspot.currentOccupancy = occupancy;
            hotspot.capacity = capacity;
            hotspot.utilization = utilization;
            
            hotspots.push_back(hotspot);
        }
    }
    
    // Sort by congestion level (highest first)
    std::sort(hotspots.begin(), hotspots.end(), 
        [](const Hotspot& a, const Hotspot& b) {
            return a.congestionLevel > b.congestionLevel;
        });
    
    return hotspots;
}

std::vector<TrafficFlowAnalyzer::FlowData> TrafficFlowAnalyzer::getFlowData(
    const std::vector<Agent*>& agents) {
    
    std::map<std::pair<NodeId, NodeId>, int> flowCounts;
    std::map<std::pair<NodeId, NodeId>, double> flowTimes;
    
    for (Agent* agent : agents) {
        if (!agent) continue;
        
        // Get agent's current path (path is stored as EdgeIds, need to convert to NodeIds)
        // For now, use origin and destination to create a simple flow
        NodeId origin = agent->getOrigin();
        NodeId destination = agent->getDestination();
        
        if (origin == destination) continue;
        
        // Create flow from origin to destination
        std::pair<NodeId, NodeId> flow = {origin, destination};
        flowCounts[flow]++;
        flowTimes[flow] += 1.0;  // Simplified
    }
    
    std::vector<FlowData> flows;
    for (const auto& pair : flowCounts) {
        FlowData data;
        data.from = pair.first.first;
        data.to = pair.first.second;
        data.agentCount = pair.second;
        data.averageTime = flowTimes[pair.first] / pair.second;
        flows.push_back(data);
    }
    
    return flows;
}

std::map<EdgeId, TrafficFlowAnalyzer::UtilizationData> TrafficFlowAnalyzer::getUtilizationHeatmap(
    const City& city) {
    
    std::map<EdgeId, UtilizationData> heatmap;
    
    int edgeCount = city.getEdgeCount();
    for (int i = 0; i < edgeCount; ++i) {
        EdgeId edgeId = city.getEdgeIdByIndex(i);
        
        int occupancy = city.occupancy(edgeId);
        int capacity = city.edgeCapacity(edgeId);
        
        if (capacity == 0) continue;
        
        UtilizationData data;
        data.edgeId = edgeId;
        data.utilization = static_cast<double>(occupancy) / capacity;
        data.occupancy = occupancy;
        data.capacity = capacity;
        
        heatmap[edgeId] = data;
    }
    
    return heatmap;
}

std::vector<TrafficFlowAnalyzer::TimePattern> TrafficFlowAnalyzer::analyzeTimePatterns(
    const std::vector<std::pair<int, double>>& history) {
    
    std::vector<TimePattern> patterns;
    
    for (const auto& entry : history) {
        TimePattern pattern;
        pattern.tick = entry.first;
        pattern.averageCongestion = entry.second;
        // These would need to be passed in or calculated separately
        pattern.activeAgents = 0;
        pattern.completedAgents = 0;
        patterns.push_back(pattern);
    }
    
    return patterns;
}

std::vector<TrafficFlowAnalyzer::Hotspot> TrafficFlowAnalyzer::getTopBottlenecks(
    const City& city, int topN) {
    
    auto hotspots = detectHotspots(city, 0.0);  // Get all edges
    
    // Sort by congestion
    std::sort(hotspots.begin(), hotspots.end(),
        [](const Hotspot& a, const Hotspot& b) {
            return a.congestionLevel > b.congestionLevel;
        });
    
    // Return top N
    if (static_cast<int>(hotspots.size()) > topN) {
        hotspots.resize(topN);
    }
    
    return hotspots;
}

double TrafficFlowAnalyzer::calculateUtilization(const City& city, EdgeId edgeId) {
    int occupancy = city.occupancy(edgeId);
    int capacity = city.edgeCapacity(edgeId);
    
    if (capacity == 0) return 0.0;
    
    return static_cast<double>(occupancy) / capacity;
}

double TrafficFlowAnalyzer::calculateCongestionLevel(int occupancy, int capacity) {
    if (capacity == 0) return 0.0;
    
    double utilization = static_cast<double>(occupancy) / capacity;
    
    // Non-linear scaling: higher utilization = exponentially higher congestion
    // This makes hotspots more visible
    return std::pow(utilization, 1.5);
}

