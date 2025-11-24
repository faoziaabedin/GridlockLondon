// code/analytics/PolicyEffectivenessAnalyzer.h
#pragma once

#include <vector>
#include <string>
#include "../core/Types.h"
#include "../core/Metrics.h"

/**
 * PolicyEffectivenessAnalyzer: Statistical analysis of policy performance
 * 
 * Provides:
 * - A/B comparison metrics
 * - Statistical significance calculations
 * - Confidence intervals
 * - Hypothesis testing
 */
class PolicyEffectivenessAnalyzer {
public:
    struct PolicyMetrics {
        std::string policyName;
        double averageTripTime;
        int totalThroughput;
        int maxEdgeLoad;
        double efficiencyScore;
        std::vector<double> tripTimeSamples;
        int sampleCount;
    };
    
    struct ComparisonResult {
        std::string policyA;
        std::string policyB;
        double difference;  // B - A
        double percentImprovement;
        bool isSignificant;  // Statistically significant?
        double pValue;
        double confidenceIntervalLower;
        double confidenceIntervalUpper;
        double confidenceLevel;  // e.g., 0.95 for 95%
    };
    
    struct HypothesisTest {
        std::string nullHypothesis;
        std::string alternativeHypothesis;
        double testStatistic;
        double pValue;
        bool rejectNull;  // true if p < 0.05
        double significanceLevel;  // e.g., 0.05
    };
    
    PolicyEffectivenessAnalyzer();
    ~PolicyEffectivenessAnalyzer() = default;
    
    /**
     * Compare two policies (A/B test)
     */
    ComparisonResult comparePolicies(const PolicyMetrics& policyA, const PolicyMetrics& policyB);
    
    /**
     * Calculate statistical significance using t-test
     */
    bool isStatisticallySignificant(const PolicyMetrics& policyA, const PolicyMetrics& policyB, double alpha = 0.05);
    
    /**
     * Calculate confidence interval for mean
     */
    std::pair<double, double> calculateConfidenceInterval(
        const std::vector<double>& samples, double confidenceLevel = 0.95);
    
    /**
     * Perform hypothesis test: Is policy B better than policy A?
     */
    HypothesisTest testHypothesis(
        const PolicyMetrics& policyA, 
        const PolicyMetrics& policyB,
        const std::string& metric = "tripTime");  // "tripTime", "throughput", "efficiency"
    
    /**
     * Calculate p-value using t-test (public for testing)
     */
    double calculatePValue(const std::vector<double>& sampleA, const std::vector<double>& sampleB);
    
    /**
     * Calculate efficiency score (0.0 to 1.0)
     */
    double calculateEfficiencyScore(double avgTripTime, int throughput, int maxLoad);
    
    /**
     * Calculate t-statistic for two samples (public for testing)
     */
    double tStatistic(const std::vector<double>& sampleA, const std::vector<double>& sampleB);
    
private:
    /**
     * Calculate mean
     */
    double mean(const std::vector<double>& values);
    
    /**
     * Calculate standard deviation
     */
    double stdDev(const std::vector<double>& values);
};

