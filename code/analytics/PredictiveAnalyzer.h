// code/analytics/PredictiveAnalyzer.h
#pragma once

#include <vector>
#include <string>
#include "../core/Types.h"

/**
 * PredictiveAnalyzer: Predictive analytics for traffic simulation
 * 
 * Provides:
 * - Trend projection
 * - Estimated completion time
 * - Bottleneck prediction
 * - Optimal agent count recommendation
 */
class PredictiveAnalyzer {
public:
    struct TrendProjection {
        std::vector<double> historicalValues;
        std::vector<double> projectedValues;
        double trendSlope;  // Positive = increasing, negative = decreasing
        double confidence;  // 0.0 to 1.0
    };
    
    struct CompletionEstimate {
        int remainingAgents;
        double estimatedTimeRemaining;  // in ticks
        double confidence;  // 0.0 to 1.0
        std::string estimateType;  // "linear", "exponential", "polynomial"
    };
    
    struct BottleneckPrediction {
        EdgeId edgeId;
        double currentUtilization;
        double predictedUtilization;  // Future utilization
        int ticksUntilBottleneck;  // When it will become a bottleneck
        double confidence;
    };
    
    struct AgentCountRecommendation {
        int currentAgentCount;
        int recommendedAgentCount;
        double expectedEfficiency;
        std::string reasoning;
    };
    
    PredictiveAnalyzer();
    ~PredictiveAnalyzer() = default;
    
    /**
     * Project trend into future
     * @param history Historical values
     * @param futureTicks Number of ticks to project
     * @return Trend projection
     */
    TrendProjection projectTrend(const std::vector<double>& history, int futureTicks = 10);
    
    /**
     * Estimate completion time for remaining agents
     */
    CompletionEstimate estimateCompletionTime(
        int remainingAgents,
        const std::vector<double>& recentCompletionTimes,
        double currentThroughput);
    
    /**
     * Predict which edges will become bottlenecks
     */
    std::vector<BottleneckPrediction> predictBottlenecks(
        const std::vector<std::pair<EdgeId, double>>& currentUtilizations,
        const std::vector<std::vector<double>>& utilizationHistory);
    
    /**
     * Recommend optimal agent count
     */
    AgentCountRecommendation recommendAgentCount(
        int currentCount,
        double currentEfficiency,
        const std::vector<std::pair<int, double>>& historicalEfficiency);
    
    /**
     * Linear regression for trend (public for testing)
     */
    std::pair<double, double> linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    
    /**
     * Exponential smoothing (public for testing)
     */
    double exponentialSmoothing(const std::vector<double>& values, double alpha = 0.3);

private:
    /**
     * Calculate trend slope
     */
    double calculateTrendSlope(const std::vector<double>& values);
};

