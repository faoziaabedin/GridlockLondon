// code/analytics/TrafficFlowAnalyzer.h
#pragma once

#include <vector>
#include <map>
#include <utility>
#include "../core/Types.h"
#include "../core/City.h"
#include "../core/Agent.h"

/**
 * TrafficFlowAnalyzer: Advanced traffic flow analysis
 * 
 * Provides:
 * - Hotspot detection (bottleneck edges)
 * - Flow diagrams data
 * - Utilization heatmap
 * - Time-based pattern analysis
 */
class TrafficFlowAnalyzer {
public:
    struct Hotspot {
        EdgeId edgeId;
        double congestionLevel;  // 0.0 to 1.0
        int currentOccupancy;
        int capacity;
        double utilization;  // occupancy / capacity
    };
    
    struct FlowData {
        NodeId from;
        NodeId to;
        int agentCount;
        double averageTime;
    };
    
    struct UtilizationData {
        EdgeId edgeId;
        double utilization;  // 0.0 to 1.0
        int occupancy;
        int capacity;
    };
    
    struct TimePattern {
        int tick;
        double averageCongestion;
        int activeAgents;
        int completedAgents;
    };
    
    TrafficFlowAnalyzer();
    ~TrafficFlowAnalyzer() = default;
    
    /**
     * Detect hotspots (bottleneck edges)
     * @param city Reference to the city
     * @param threshold Utilization threshold (default 0.8 = 80%)
     * @return Vector of hotspots sorted by congestion level
     */
    std::vector<Hotspot> detectHotspots(const City& city, double threshold = 0.8);
    
    /**
     * Get flow data for Sankey diagram
     * @param agents Vector of agents
     * @return Vector of flow data (from -> to with agent counts)
     */
    std::vector<FlowData> getFlowData(const std::vector<Agent*>& agents);
    
    /**
     * Get utilization heatmap data
     * @param city Reference to the city
     * @return Map of edgeId -> utilization data
     */
    std::map<EdgeId, UtilizationData> getUtilizationHeatmap(const City& city);
    
    /**
     * Analyze time-based patterns
     * @param history Vector of (tick, congestion) pairs
     * @return Vector of time patterns
     */
    std::vector<TimePattern> analyzeTimePatterns(const std::vector<std::pair<int, double>>& history);
    
    /**
     * Get top N bottlenecks
     */
    std::vector<Hotspot> getTopBottlenecks(const City& city, int topN = 10);
    
    /**
     * Calculate edge utilization percentage
     */
    double calculateUtilization(const City& city, EdgeId edgeId);
    
private:
    /**
     * Calculate congestion level (0.0 to 1.0)
     */
    double calculateCongestionLevel(int occupancy, int capacity);
};

