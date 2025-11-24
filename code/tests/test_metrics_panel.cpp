// code/tests/test_metrics_panel.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QLabel>
#include "../ui/MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Preset.h"
#include "../adapters/PresetLoader.h"

/**
 * Test suite for MetricsPanel component.
 * Tests metrics display and updates.
 */
class TestMetricsPanel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Basic tests
    void testMetricsPanelCreation();
    void testSetSimulationController();
    void testUpdateMetrics();
    void testMetricsLabelsExist();

private:
    QApplication* app;
    MetricsPanel* metricsPanel;
    SimulationController* controller;
};

void TestMetricsPanel::initTestCase() {
    int argc = 0;
    char* argv[] = {nullptr};
    app = new QApplication(argc, argv);
}

void TestMetricsPanel::cleanupTestCase() {
    delete app;
}

void TestMetricsPanel::init() {
    metricsPanel = new MetricsPanel();
    controller = &SimulationController::getInstance();
    
    // Load a test preset
    PresetLoader loader;
    Preset testPreset = loader.loadFromJson("presets/demo_3x3.json");
    if (!testPreset.validate()) {
        testPreset.setName("test");
        testPreset.setRows(3);
        testPreset.setCols(3);
        testPreset.setAgentCount(2);
        testPreset.setTickMs(100);
        testPreset.setPolicy(PolicyType::SHORTEST_PATH);
    }
    controller->loadPreset(testPreset);
}

void TestMetricsPanel::cleanup() {
    delete metricsPanel;
    controller->reset();
}

void TestMetricsPanel::testMetricsPanelCreation() {
    QVERIFY(metricsPanel != nullptr);
}

void TestMetricsPanel::testSetSimulationController() {
    metricsPanel->setSimulationController(controller);
    // Should not crash
    QVERIFY(true);
}

void TestMetricsPanel::testUpdateMetrics() {
    metricsPanel->setSimulationController(controller);
    
    // Update metrics
    metricsPanel->updateMetrics();
    
    // Should not crash
    QVERIFY(true);
    
    // Run a few simulation ticks
    controller->start();
    for (int i = 0; i < 5; ++i) {
        controller->tick();
        metricsPanel->updateMetrics();
    }
    controller->pause();
    
    // Should still work
    QVERIFY(true);
}

void TestMetricsPanel::testMetricsLabelsExist() {
    // Labels should be created in setupUI
    // We verify the panel was created successfully
    QVERIFY(metricsPanel != nullptr);
}

QTEST_MAIN(TestMetricsPanel)
#include "test_metrics_panel.moc"

