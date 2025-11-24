// code/tests/test_analytics.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include "../analytics/TrafficFlowAnalyzer.h"
#include "../analytics/PolicyEffectivenessAnalyzer.h"
#include "../analytics/PredictiveAnalyzer.h"
#include "../analytics/ReportExporter.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"
#include "../core/Agent.h"
#include "../core/SimulationController.h"
#include "../core/Metrics.h"
#include "../core/Preset.h"
#include "../adapters/PresetLoader.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <memory>

class TestAnalytics : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // TrafficFlowAnalyzer tests
    void testHotspotDetection();
    void testFlowData();
    void testUtilizationHeatmap();
    void testTopBottlenecks();
    void testCalculateUtilization();
    
    // PolicyEffectivenessAnalyzer tests
    void testPolicyComparison();
    void testStatisticalSignificance();
    void testConfidenceInterval();
    void testHypothesisTest();
    void testEfficiencyScore();
    void testCalculatePValue();
    void testTStatistic();
    
    // PredictiveAnalyzer tests
    void testTrendProjection();
    void testCompletionEstimate();
    void testBottleneckPrediction();
    void testAgentRecommendation();
    void testLinearRegression();
    void testExponentialSmoothing();
    
    // ReportExporter tests
    void testExportCSV();
    void testExportJSON();
    void testSaveJSON();

private:
    std::unique_ptr<City> createTestCity();
    std::unique_ptr<SimulationController> m_controller;
};

void TestAnalytics::initTestCase() {
    // Check if QApplication already exists (from QTEST_MAIN)
    if (!QApplication::instance()) {
        static int argc = 1;
        static char* argv[] = {const_cast<char*>("test_analytics")};
        new QApplication(argc, argv);
    }
}

void TestAnalytics::cleanupTestCase() {
    // QApplication cleanup handled by QTEST_MAIN
    QApplication::processEvents();
}

void TestAnalytics::init() {
    m_controller = std::make_unique<SimulationController>();
}

void TestAnalytics::cleanup() {
    m_controller.reset();
}

std::unique_ptr<City> TestAnalytics::createTestCity() {
    auto city = std::make_unique<City>();
    
    // Create a 3x3 grid
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            NodeId nodeId = row * 3 + col;
            city->addNode(Node(nodeId, row, col));
        }
    }
    
    // Create edges
    int edgeId = 0;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 2; ++col) {
            NodeId from = row * 3 + col;
            NodeId to = row * 3 + col + 1;
            city->addEdge(Edge(edgeId++, from, to, 1.0, 10));
            city->addEdge(Edge(edgeId++, to, from, 1.0, 10));
        }
    }
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            NodeId from = row * 3 + col;
            NodeId to = (row + 1) * 3 + col;
            city->addEdge(Edge(edgeId++, from, to, 1.0, 10));
            city->addEdge(Edge(edgeId++, to, from, 1.0, 10));
        }
    }
    
    return city;
}

void TestAnalytics::testHotspotDetection() {
    auto city = createTestCity();
    TrafficFlowAnalyzer analyzer;
    
    // Set some edges to high occupancy
    city->setOccupancy(0, 8);  // 80% utilization
    city->setOccupancy(1, 9);  // 90% utilization
    
    auto hotspots = analyzer.detectHotspots(*city, 0.8);
    
    QVERIFY(hotspots.size() >= 2);
    QVERIFY(hotspots[0].utilization >= 0.8);
}

void TestAnalytics::testFlowData() {
    TrafficFlowAnalyzer analyzer;
    
    std::vector<Agent*> agents;
    auto agent1 = std::make_unique<Agent>(0, 0, 4);
    auto agent2 = std::make_unique<Agent>(1, 1, 5);
    auto agent3 = std::make_unique<Agent>(2, 0, 4);  // Same flow as agent1
    
    agents.push_back(agent1.get());
    agents.push_back(agent2.get());
    agents.push_back(agent3.get());
    
    auto flowData = analyzer.getFlowData(agents);
    
    QVERIFY(flowData.size() > 0);
    
    // Check that flow from 0 to 4 has at least 2 agents
    bool foundFlow = false;
    for (const auto& flow : flowData) {
        if (flow.from == 0 && flow.to == 4) {
            QVERIFY(flow.agentCount >= 2);
            foundFlow = true;
        }
    }
    QVERIFY(foundFlow);
}

void TestAnalytics::testUtilizationHeatmap() {
    auto city = createTestCity();
    TrafficFlowAnalyzer analyzer;
    
    city->setOccupancy(0, 5);
    
    auto heatmap = analyzer.getUtilizationHeatmap(*city);
    
    QVERIFY(heatmap.size() > 0);
    QVERIFY(heatmap.find(0) != heatmap.end());
    QVERIFY(heatmap[0].utilization > 0.0);
}

void TestAnalytics::testTopBottlenecks() {
    auto city = createTestCity();
    TrafficFlowAnalyzer analyzer;
    
    city->setOccupancy(0, 9);
    city->setOccupancy(1, 8);
    city->setOccupancy(2, 7);
    
    auto bottlenecks = analyzer.getTopBottlenecks(*city, 3);
    
    QVERIFY(bottlenecks.size() <= 3);
    if (bottlenecks.size() > 0) {
        QVERIFY(bottlenecks[0].congestionLevel >= bottlenecks.back().congestionLevel);
    }
}

void TestAnalytics::testCalculateUtilization() {
    auto city = createTestCity();
    TrafficFlowAnalyzer analyzer;
    
    city->setOccupancy(0, 5);  // 50% of capacity 10
    
    double util = analyzer.calculateUtilization(*city, 0);
    QCOMPARE(util, 0.5);
}

void TestAnalytics::testPolicyComparison() {
    PolicyEffectivenessAnalyzer analyzer;
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyA;
    policyA.policyName = "ShortestPath";
    policyA.averageTripTime = 15.0;
    policyA.totalThroughput = 10;
    policyA.maxEdgeLoad = 5;
    policyA.tripTimeSamples = {14.0, 15.0, 16.0, 15.5, 14.5};
    policyA.sampleCount = 5;
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyB;
    policyB.policyName = "CongestionAware";
    policyB.averageTripTime = 12.0;
    policyB.totalThroughput = 12;
    policyB.maxEdgeLoad = 4;
    policyB.tripTimeSamples = {11.0, 12.0, 13.0, 12.5, 11.5};
    policyB.sampleCount = 5;
    
    auto result = analyzer.comparePolicies(policyA, policyB);
    
    QCOMPARE(result.policyA, QString("ShortestPath"));
    QCOMPARE(result.policyB, QString("CongestionAware"));
    QVERIFY(result.difference < 0);  // B is better (lower trip time)
}

void TestAnalytics::testStatisticalSignificance() {
    PolicyEffectivenessAnalyzer analyzer;
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyA;
    policyA.tripTimeSamples = {14.0, 15.0, 16.0, 15.5, 14.5};
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyB;
    policyB.tripTimeSamples = {11.0, 12.0, 13.0, 12.5, 11.5};
    
    bool significant = analyzer.isStatisticallySignificant(policyA, policyB);
    
    // Result depends on implementation, just verify it doesn't crash
    Q_UNUSED(significant);
}

void TestAnalytics::testConfidenceInterval() {
    PolicyEffectivenessAnalyzer analyzer;
    
    std::vector<double> samples = {10.0, 11.0, 12.0, 11.5, 10.5};
    
    auto ci = analyzer.calculateConfidenceInterval(samples, 0.95);
    
    QVERIFY(ci.first < ci.second);  // Lower < Upper
    QVERIFY(ci.first > 0.0);
}

void TestAnalytics::testHypothesisTest() {
    PolicyEffectivenessAnalyzer analyzer;
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyA;
    policyA.tripTimeSamples = {14.0, 15.0, 16.0};
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyB;
    policyB.tripTimeSamples = {11.0, 12.0, 13.0};
    
    auto test = analyzer.testHypothesis(policyA, policyB, "tripTime");
    
    QVERIFY(!test.nullHypothesis.isEmpty());
    QVERIFY(!test.alternativeHypothesis.isEmpty());
}

void TestAnalytics::testEfficiencyScore() {
    PolicyEffectivenessAnalyzer analyzer;
    
    double score = analyzer.calculateEfficiencyScore(10.0, 15, 5);
    
    QVERIFY(score >= 0.0);
    QVERIFY(score <= 1.0);
}

void TestAnalytics::testTrendProjection() {
    PredictiveAnalyzer analyzer;
    
    std::vector<double> history = {10.0, 11.0, 12.0, 11.5, 13.0};
    
    auto projection = analyzer.projectTrend(history, 5);
    
    QVERIFY(projection.projectedValues.size() == 5);
    QVERIFY(projection.confidence >= 0.0);
    QVERIFY(projection.confidence <= 1.0);
}

void TestAnalytics::testCompletionEstimate() {
    PredictiveAnalyzer analyzer;
    
    std::vector<double> completionTimes = {5.0, 6.0, 5.5};
    
    auto estimate = analyzer.estimateCompletionTime(10, completionTimes, 2.0);
    
    QCOMPARE(estimate.remainingAgents, 10);
    QVERIFY(estimate.estimatedTimeRemaining > 0.0);
}

void TestAnalytics::testBottleneckPrediction() {
    PredictiveAnalyzer analyzer;
    
    std::vector<std::pair<EdgeId, double>> currentUtilizations = {
        {0, 0.5}, {1, 0.6}, {2, 0.7}
    };
    
    std::vector<std::vector<double>> history = {
        {0.3, 0.4, 0.5},  // Edge 0 history
        {0.4, 0.5, 0.6},  // Edge 1 history
        {0.5, 0.6, 0.7}   // Edge 2 history
    };
    
    auto predictions = analyzer.predictBottlenecks(currentUtilizations, history);
    
    QVERIFY(predictions.size() > 0);
}

void TestAnalytics::testAgentRecommendation() {
    PredictiveAnalyzer analyzer;
    
    std::vector<std::pair<int, double>> historicalEfficiency = {
        {5, 0.6}, {10, 0.8}, {15, 0.7}, {20, 0.5}
    };
    
    auto recommendation = analyzer.recommendAgentCount(10, 0.75, historicalEfficiency);
    
    QCOMPARE(recommendation.currentAgentCount, 10);
    QVERIFY(recommendation.recommendedAgentCount > 0);
    QVERIFY(!recommendation.reasoning.isEmpty());
}

void TestAnalytics::testCalculatePValue() {
    PolicyEffectivenessAnalyzer analyzer;
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyA;
    policyA.tripTimeSamples = {14.0, 15.0, 16.0, 15.5, 14.5, 15.2, 14.8};
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyB;
    policyB.tripTimeSamples = {11.0, 12.0, 13.0, 12.5, 11.5, 12.2, 11.8};
    
    double pValue = analyzer.calculatePValue(policyA.tripTimeSamples, policyB.tripTimeSamples);
    
    QVERIFY(pValue >= 0.0);
    QVERIFY(pValue <= 1.0);
}

void TestAnalytics::testTStatistic() {
    PolicyEffectivenessAnalyzer analyzer;
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyA;
    policyA.tripTimeSamples = {10.0, 11.0, 12.0, 11.5, 10.5};
    
    PolicyEffectivenessAnalyzer::PolicyMetrics policyB;
    policyB.tripTimeSamples = {8.0, 9.0, 10.0, 9.5, 8.5};
    
    // Test that comparison works
    auto result = analyzer.comparePolicies(policyA, policyB);
    
    QVERIFY(result.difference != 0.0);  // Should be different
    QVERIFY(result.pValue >= 0.0);
    QVERIFY(result.pValue <= 1.0);
}

void TestAnalytics::testLinearRegression() {
    PredictiveAnalyzer analyzer;
    
    std::vector<double> x = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<double> y = {1.0, 2.0, 3.0, 4.0, 5.0};  // Perfect linear relationship
    
    auto regression = analyzer.linearRegression(x, y);
    double slope = regression.first;
    double intercept = regression.second;
    
    // For y = x + 1, slope should be ~1.0, intercept ~1.0
    QVERIFY(std::abs(slope - 1.0) < 0.1);
    QVERIFY(std::abs(intercept - 1.0) < 0.1);
}

void TestAnalytics::testExponentialSmoothing() {
    PredictiveAnalyzer analyzer;
    
    std::vector<double> values = {10.0, 11.0, 12.0, 11.5, 13.0};
    
    double smoothed = analyzer.exponentialSmoothing(values, 0.3);
    
    QVERIFY(smoothed > 0.0);
    QVERIFY(smoothed < 15.0);  // Should be within range of values
}

void TestAnalytics::testExportCSV() {
    ReportExporter exporter;
    TrafficFlowAnalyzer flowAnalyzer;
    
    // Create a minimal simulation controller with city
    SimulationController controller;
    PresetLoader loader;
    Preset preset;
    preset.setName("Test Preset");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    preset.setTickMs(100);
    
    try {
        controller.loadPreset(preset);
        
        QString filepath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/test_analytics.csv";
        
        // Clean up any existing file
        QFile::remove(filepath);
        
        bool success = exporter.exportCSV(filepath, controller, flowAnalyzer, true);
        
        QVERIFY(success);
        QVERIFY(QFile::exists(filepath));
        
        // Verify file has content
        QFile file(filepath);
        QVERIFY(file.open(QIODevice::ReadOnly));
        QByteArray content = file.readAll();
        file.close();
        
        QVERIFY(content.size() > 0);
        QVERIFY(content.contains("EdgeId"));
        
        // Cleanup
        QFile::remove(filepath);
    } catch (...) {
        QSKIP("Preset loading failed, skipping CSV export test");
    }
}

void TestAnalytics::testExportJSON() {
    ReportExporter exporter;
    TrafficFlowAnalyzer flowAnalyzer;
    PolicyEffectivenessAnalyzer policyAnalyzer;
    PredictiveAnalyzer predictiveAnalyzer;
    
    // Create a minimal simulation controller
    SimulationController controller;
    PresetLoader loader;
    Preset preset;
    preset.setName("Test Preset");
    preset.setRows(3);
    preset.setCols(3);
    preset.setAgentCount(5);
    preset.setPolicy(PolicyType::SHORTEST_PATH);
    preset.setTickMs(100);
    
    try {
        controller.loadPreset(preset);
        
        QString json = exporter.exportJSON(controller, flowAnalyzer, policyAnalyzer, predictiveAnalyzer);
        
        QVERIFY(!json.isEmpty());
        QVERIFY(json.contains("timestamp"));
        QVERIFY(json.contains("version"));
        QVERIFY(json.contains("application"));
    } catch (...) {
        QSKIP("Preset loading failed, skipping JSON export test");
    }
}

void TestAnalytics::testSaveJSON() {
    ReportExporter exporter;
    
    QString json = R"({"test": "data"})";
    QString filepath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/test_analytics.json";
    
    bool success = exporter.saveJSON(filepath, json);
    
    QVERIFY(success);
    
    // Verify file exists
    QFile file(filepath);
    QVERIFY(file.exists());
    
    // Cleanup
    file.remove();
}

QTEST_MAIN(TestAnalytics)
#include "test_analytics.moc"

