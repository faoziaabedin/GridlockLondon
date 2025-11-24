// code/analytics/PolicyEffectivenessAnalyzer.cpp
#include "PolicyEffectivenessAnalyzer.h"
#include <algorithm>
#include <cmath>
#include <numeric>

PolicyEffectivenessAnalyzer::PolicyEffectivenessAnalyzer() {
}

PolicyEffectivenessAnalyzer::ComparisonResult PolicyEffectivenessAnalyzer::comparePolicies(
    const PolicyMetrics& policyA, const PolicyMetrics& policyB) {
    
    ComparisonResult result;
    result.policyA = policyA.policyName;
    result.policyB = policyB.policyName;
    
    // Calculate difference
    result.difference = policyB.averageTripTime - policyA.averageTripTime;
    result.percentImprovement = ((policyA.averageTripTime - policyB.averageTripTime) / policyA.averageTripTime) * 100.0;
    
    // Statistical significance
    result.isSignificant = isStatisticallySignificant(policyA, policyB);
    result.pValue = calculatePValue(policyA.tripTimeSamples, policyB.tripTimeSamples);
    
    // Confidence intervals
    auto ciA = calculateConfidenceInterval(policyA.tripTimeSamples);
    auto ciB = calculateConfidenceInterval(policyB.tripTimeSamples);
    
    // Combined confidence interval for difference
    double diffMean = result.difference;
    double diffStdErr = std::sqrt(
        std::pow(stdDev(policyA.tripTimeSamples), 2) / policyA.tripTimeSamples.size() +
        std::pow(stdDev(policyB.tripTimeSamples), 2) / policyB.tripTimeSamples.size()
    );
    
    // 95% confidence interval (t-value ≈ 1.96 for large samples)
    double tValue = 1.96;  // Simplified, should use t-distribution
    result.confidenceIntervalLower = diffMean - tValue * diffStdErr;
    result.confidenceIntervalUpper = diffMean + tValue * diffStdErr;
    result.confidenceLevel = 0.95;
    
    return result;
}

bool PolicyEffectivenessAnalyzer::isStatisticallySignificant(
    const PolicyMetrics& policyA, const PolicyMetrics& policyB, double alpha) {
    
    double pValue = calculatePValue(policyA.tripTimeSamples, policyB.tripTimeSamples);
    return pValue < alpha;
}

std::pair<double, double> PolicyEffectivenessAnalyzer::calculateConfidenceInterval(
    const std::vector<double>& samples, double confidenceLevel) {
    
    if (samples.empty()) {
        return {0.0, 0.0};
    }
    
    double sampleMean = mean(samples);
    double sampleStdDev = stdDev(samples);
    double n = static_cast<double>(samples.size());
    
    // Standard error
    double stdError = sampleStdDev / std::sqrt(n);
    
    // t-value for confidence level (simplified: using 1.96 for 95% CI)
    double tValue = 1.96;  // Should use t-distribution based on degrees of freedom
    
    double margin = tValue * stdError;
    
    return {sampleMean - margin, sampleMean + margin};
}

PolicyEffectivenessAnalyzer::HypothesisTest PolicyEffectivenessAnalyzer::testHypothesis(
    const PolicyMetrics& policyA, const PolicyMetrics& policyB, const std::string& metric) {
    
    HypothesisTest test;
    test.significanceLevel = 0.05;
    
    if (metric == "tripTime") {
        test.nullHypothesis = "Policy B average trip time = Policy A average trip time";
        test.alternativeHypothesis = "Policy B average trip time < Policy A average trip time";
        
        test.testStatistic = tStatistic(policyA.tripTimeSamples, policyB.tripTimeSamples);
        test.pValue = calculatePValue(policyA.tripTimeSamples, policyB.tripTimeSamples);
    } else if (metric == "throughput") {
        // Compare throughput (higher is better)
        double diff = static_cast<double>(policyB.totalThroughput) - policyA.totalThroughput;
        test.testStatistic = diff;
        test.pValue = 0.05;  // Simplified
        test.nullHypothesis = "Policy B throughput = Policy A throughput";
        test.alternativeHypothesis = "Policy B throughput > Policy A throughput";
    } else {
        // Efficiency score
        double diff = policyB.efficiencyScore - policyA.efficiencyScore;
        test.testStatistic = diff;
        test.pValue = 0.05;  // Simplified
        test.nullHypothesis = "Policy B efficiency = Policy A efficiency";
        test.alternativeHypothesis = "Policy B efficiency > Policy A efficiency";
    }
    
    test.rejectNull = test.pValue < test.significanceLevel;
    
    return test;
}

double PolicyEffectivenessAnalyzer::calculatePValue(
    const std::vector<double>& sampleA, const std::vector<double>& sampleB) {
    
    if (sampleA.empty() || sampleB.empty()) {
        return 1.0;  // Cannot determine significance
    }
    
    // Simplified t-test p-value calculation
    // In production, use proper statistical library
    
    double tStat = tStatistic(sampleA, sampleB);
    double degreesOfFreedom = static_cast<double>(sampleA.size() + sampleB.size() - 2);
    
    // Simplified p-value: use absolute t-statistic
    // For proper implementation, use t-distribution CDF
    double absT = std::abs(tStat);
    
    // Approximate p-value (simplified)
    // Real implementation would use t-distribution
    if (absT > 2.0) {
        return 0.05;  // Significant
    } else if (absT > 1.5) {
        return 0.10;  // Marginally significant
    } else {
        return 0.50;  // Not significant
    }
}

double PolicyEffectivenessAnalyzer::calculateEfficiencyScore(
    double avgTripTime, int throughput, int maxLoad) {
    
    // Normalize metrics to 0-1 scale
    // Lower trip time = better, higher throughput = better, lower max load = better
    
    // Normalize trip time (assume max reasonable is 100 steps)
    double normalizedTripTime = 1.0 - std::min(avgTripTime / 100.0, 1.0);
    
    // Normalize throughput (assume max is 100 agents)
    double normalizedThroughput = std::min(static_cast<double>(throughput) / 100.0, 1.0);
    
    // Normalize max load (assume max capacity is 20)
    double normalizedLoad = 1.0 - std::min(static_cast<double>(maxLoad) / 20.0, 1.0);
    
    // Weighted average
    return (normalizedTripTime * 0.4 + normalizedThroughput * 0.3 + normalizedLoad * 0.3);
}

double PolicyEffectivenessAnalyzer::mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / static_cast<double>(values.size());
}

double PolicyEffectivenessAnalyzer::stdDev(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    double m = mean(values);
    double sumSquaredDiff = 0.0;
    
    for (double value : values) {
        double diff = value - m;
        sumSquaredDiff += diff * diff;
    }
    
    double variance = sumSquaredDiff / static_cast<double>(values.size() - 1);
    return std::sqrt(variance);
}

double PolicyEffectivenessAnalyzer::tStatistic(
    const std::vector<double>& sampleA, const std::vector<double>& sampleB) {
    
    double meanA = mean(sampleA);
    double meanB = mean(sampleB);
    double stdDevA = stdDev(sampleA);
    double stdDevB = stdDev(sampleB);
    
    double nA = static_cast<double>(sampleA.size());
    double nB = static_cast<double>(sampleB.size());
    
    // Pooled standard error
    double pooledStdErr = std::sqrt(
        (stdDevA * stdDevA / nA) + (stdDevB * stdDevB / nB)
    );
    
    if (pooledStdErr == 0.0) return 0.0;
    
    return (meanB - meanA) / pooledStdErr;
}

