// code/analytics/PredictiveAnalyzer.cpp
#include "PredictiveAnalyzer.h"
#include <algorithm>
#include <cmath>
#include <numeric>

PredictiveAnalyzer::PredictiveAnalyzer() {
}

PredictiveAnalyzer::TrendProjection PredictiveAnalyzer::projectTrend(
    const std::vector<double>& history, int futureTicks) {
    
    TrendProjection projection;
    projection.historicalValues = history;
    
    if (history.size() < 2) {
        // Not enough data
        projection.trendSlope = 0.0;
        projection.confidence = 0.0;
        return projection;
    }
    
    // Calculate trend using linear regression
    std::vector<double> xValues;
    for (size_t i = 0; i < history.size(); ++i) {
        xValues.push_back(static_cast<double>(i));
    }
    
    auto regression = linearRegression(xValues, history);
    double slope = regression.first;
    double intercept = regression.second;
    
    projection.trendSlope = slope;
    
    // Project future values
    for (int i = 0; i < futureTicks; ++i) {
        double x = static_cast<double>(history.size() + i);
        double projected = slope * x + intercept;
        projection.projectedValues.push_back(projected);
    }
    
    // Calculate confidence based on data quality
    double variance = 0.0;
    for (size_t i = 0; i < history.size(); ++i) {
        double predicted = slope * static_cast<double>(i) + intercept;
        double error = history[i] - predicted;
        variance += error * error;
    }
    variance /= static_cast<double>(history.size());
    
    // Higher variance = lower confidence
    projection.confidence = std::max(0.0, 1.0 - std::min(variance / 100.0, 1.0));
    
    return projection;
}

PredictiveAnalyzer::CompletionEstimate PredictiveAnalyzer::estimateCompletionTime(
    int remainingAgents,
    const std::vector<double>& recentCompletionTimes,
    double currentThroughput) {
    
    CompletionEstimate estimate;
    estimate.remainingAgents = remainingAgents;
    
    if (recentCompletionTimes.empty() || currentThroughput <= 0) {
        estimate.estimatedTimeRemaining = 0.0;
        estimate.confidence = 0.0;
        estimate.estimateType = "insufficient_data";
        return estimate;
    }
    
    // Simple linear estimate
    double avgCompletionTime = std::accumulate(recentCompletionTimes.begin(), 
                                               recentCompletionTimes.end(), 0.0) 
                               / static_cast<double>(recentCompletionTimes.size());
    
    estimate.estimatedTimeRemaining = static_cast<double>(remainingAgents) / currentThroughput;
    estimate.confidence = std::min(static_cast<double>(recentCompletionTimes.size()) / 10.0, 1.0);
    estimate.estimateType = "linear";
    
    return estimate;
}

std::vector<PredictiveAnalyzer::BottleneckPrediction> PredictiveAnalyzer::predictBottlenecks(
    const std::vector<std::pair<EdgeId, double>>& currentUtilizations,
    const std::vector<std::vector<double>>& utilizationHistory) {
    
    std::vector<BottleneckPrediction> predictions;
    
    for (const auto& current : currentUtilizations) {
        EdgeId edgeId = current.first;
        double currentUtil = current.second;
        
        // Find history for this edge
        std::vector<double> edgeHistory;
        for (const auto& history : utilizationHistory) {
            if (edgeHistory.size() < history.size()) {
                edgeHistory = history;  // Simplified: use first matching history
            }
        }
        
        if (edgeHistory.empty()) continue;
        
        // Calculate trend
        double trendSlope = calculateTrendSlope(edgeHistory);
        
        BottleneckPrediction prediction;
        prediction.edgeId = edgeId;
        prediction.currentUtilization = currentUtil;
        prediction.predictedUtilization = currentUtil + (trendSlope * 10.0);  // Project 10 ticks ahead
        
        // Estimate when it will become a bottleneck (utilization > 0.8)
        if (trendSlope > 0 && currentUtil < 0.8) {
            double ticksToBottleneck = (0.8 - currentUtil) / trendSlope;
            prediction.ticksUntilBottleneck = static_cast<int>(ticksToBottleneck);
        } else {
            prediction.ticksUntilBottleneck = -1;  // Already a bottleneck or not trending
        }
        
        prediction.confidence = std::min(static_cast<double>(edgeHistory.size()) / 20.0, 1.0);
        
        predictions.push_back(prediction);
    }
    
    // Sort by predicted utilization
    std::sort(predictions.begin(), predictions.end(),
        [](const BottleneckPrediction& a, const BottleneckPrediction& b) {
            return a.predictedUtilization > b.predictedUtilization;
        });
    
    return predictions;
}

PredictiveAnalyzer::AgentCountRecommendation PredictiveAnalyzer::recommendAgentCount(
    int currentCount,
    double currentEfficiency,
    const std::vector<std::pair<int, double>>& historicalEfficiency) {
    
    AgentCountRecommendation recommendation;
    recommendation.currentAgentCount = currentCount;
    
    if (historicalEfficiency.empty()) {
        recommendation.recommendedAgentCount = currentCount;
        recommendation.expectedEfficiency = currentEfficiency;
        recommendation.reasoning = "Insufficient data for recommendation";
        return recommendation;
    }
    
    // Find optimal agent count (highest efficiency in history)
    auto best = std::max_element(historicalEfficiency.begin(), historicalEfficiency.end(),
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second < b.second;
        });
    
    if (best != historicalEfficiency.end()) {
        recommendation.recommendedAgentCount = best->first;
        recommendation.expectedEfficiency = best->second;
        
        if (recommendation.recommendedAgentCount > currentCount) {
            recommendation.reasoning = "Increasing agent count may improve efficiency";
        } else if (recommendation.recommendedAgentCount < currentCount) {
            recommendation.reasoning = "Reducing agent count may reduce congestion";
        } else {
            recommendation.reasoning = "Current agent count appears optimal";
        }
    } else {
        recommendation.recommendedAgentCount = currentCount;
        recommendation.expectedEfficiency = currentEfficiency;
        recommendation.reasoning = "Cannot determine optimal count";
    }
    
    return recommendation;
}

std::pair<double, double> PredictiveAnalyzer::linearRegression(
    const std::vector<double>& x, const std::vector<double>& y) {
    
    if (x.size() != y.size() || x.size() < 2) {
        return {0.0, 0.0};
    }
    
    double n = static_cast<double>(x.size());
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumX2 = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    
    // Slope: (n*sumXY - sumX*sumY) / (n*sumX2 - sumX*sumX)
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0.0) {
        return {0.0, 0.0};
    }
    
    double slope = (n * sumXY - sumX * sumY) / denominator;
    double intercept = (sumY - slope * sumX) / n;
    
    return {slope, intercept};
}

double PredictiveAnalyzer::calculateTrendSlope(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    std::vector<double> xValues;
    for (size_t i = 0; i < values.size(); ++i) {
        xValues.push_back(static_cast<double>(i));
    }
    
    auto regression = linearRegression(xValues, values);
    return regression.first;  // Return slope
}

double PredictiveAnalyzer::exponentialSmoothing(
    const std::vector<double>& values, double alpha) {
    
    if (values.empty()) return 0.0;
    if (values.size() == 1) return values[0];
    
    double smoothed = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
        smoothed = alpha * values[i] + (1.0 - alpha) * smoothed;
    }
    
    return smoothed;
}

