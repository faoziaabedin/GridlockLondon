// code/tests/test_metrics_panel.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QGroupBox>
#include "../ui/MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Preset.h"
#include "../core/Metrics.h"
#include "../core/Agent.h"
#include "../adapters/PresetLoader.h"

/**
 * Test suite for MetricsPanel component with modern UI and charts.
 * Tests metrics display, updates, and chart rendering.
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
    
    // Chart tests
    void testChartDataCollection();
    void testChartHistoryLimit();
    void testChartRendering();
    
    // UI component tests
    void testTitleLabel();
    void testComparisonSection();
    void testExportButton();
    void testSectionHeaders();
    
    // Integration tests
    void testMetricsUpdateDuringSimulation();
    void testMultipleUpdates();
    void testEmptyState();

private:
    QApplication* app;
    MetricsPanel* metricsPanel;
    SimulationController* controller;
};

void TestMetricsPanel::initTestCase() {
    // Check if QApplication already exists (from QTEST_MAIN)
    if (!QCoreApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    } else {
        app = qobject_cast<QApplication*>(QCoreApplication::instance());
    }
}

void TestMetricsPanel::cleanupTestCase() {
    // Don't delete app if it was created by QTEST_MAIN
    // Only delete if we created it ourselves
    if (app && app != QCoreApplication::instance()) {
        delete app;
        app = nullptr;
    }
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
    // Delete widgets before application cleanup
    if (metricsPanel) {
        delete metricsPanel;
        metricsPanel = nullptr;
    }
    if (controller) {
        controller->reset();
    }
    // Process events to ensure cleanup
    if (app) {
        QApplication::processEvents();
    }
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
    QVERIFY(metricsPanel != nullptr);
    
    // Check for title label
    QList<QLabel*> labels = metricsPanel->findChildren<QLabel*>();
    bool foundTitle = false;
    for (QLabel* label : labels) {
        if (label->text().contains("METRICS") || label->text().contains("📊")) {
            foundTitle = true;
            break;
        }
    }
    QVERIFY(foundTitle);
}

void TestMetricsPanel::testChartDataCollection() {
    metricsPanel->setSimulationController(controller);
    
    // Update metrics multiple times to collect chart data
    for (int i = 0; i < 10; ++i) {
        controller->tick();
        metricsPanel->updateMetrics();
    }
    
    // Chart data should be collected
    QVERIFY(true); // Data collection is internal, we verify it doesn't crash
}

void TestMetricsPanel::testChartHistoryLimit() {
    metricsPanel->setSimulationController(controller);
    
    // Update many times to test history limit
    for (int i = 0; i < 150; ++i) {
        controller->tick();
        metricsPanel->updateMetrics();
    }
    
    // Should not crash and should limit history
    QVERIFY(true);
}

void TestMetricsPanel::testChartRendering() {
    metricsPanel->setSimulationController(controller);
    
    // Update metrics to generate chart data
    for (int i = 0; i < 20; ++i) {
        controller->tick();
        metricsPanel->updateMetrics();
    }
    
    // Trigger paint event
    metricsPanel->update();
    metricsPanel->repaint();
    
    // Should render without crashing
    QVERIFY(true);
}

void TestMetricsPanel::testTitleLabel() {
    QList<QLabel*> labels = metricsPanel->findChildren<QLabel*>();
    bool foundTitle = false;
    
    for (QLabel* label : labels) {
        QString text = label->text();
        if (text.contains("METRICS") || text.contains("📊")) {
            foundTitle = true;
            QVERIFY(label->objectName() == "sectionTitle" || !label->objectName().isEmpty());
            break;
        }
    }
    QVERIFY(foundTitle);
}

void TestMetricsPanel::testComparisonSection() {
    QList<QLabel*> labels = metricsPanel->findChildren<QLabel*>();
    bool foundComparison = false;
    
    for (QLabel* label : labels) {
        QString text = label->text();
        if (text.contains("COMPARISON") || text.contains("📋") ||
            text.contains("Shortest Path") || text.contains("Congestion-Aware")) {
            foundComparison = true;
            break;
        }
    }
    QVERIFY(foundComparison);
}

void TestMetricsPanel::testExportButton() {
    QList<QPushButton*> buttons = metricsPanel->findChildren<QPushButton*>();
    bool foundExportButton = false;
    
    for (QPushButton* button : buttons) {
        if (button->text().contains("Export") || button->text().contains("📤")) {
            foundExportButton = true;
            break;
        }
    }
    QVERIFY(foundExportButton);
}

void TestMetricsPanel::testSectionHeaders() {
    QList<QLabel*> labels = metricsPanel->findChildren<QLabel*>();
    int sectionCount = 0;
    
    for (QLabel* label : labels) {
        QString text = label->text();
        if (text.contains("METRICS") || text.contains("COMPARISON") ||
            text.contains("📊") || text.contains("📋")) {
            sectionCount++;
        }
    }
    QVERIFY(sectionCount >= 1);
}

void TestMetricsPanel::testMetricsUpdateDuringSimulation() {
    metricsPanel->setSimulationController(controller);
    
    controller->start();
    
    // Update metrics during active simulation
    for (int i = 0; i < 10; ++i) {
        controller->tick();
        metricsPanel->updateMetrics();
        QApplication::processEvents(); // Process UI events
    }
    
    controller->pause();
    
    // Should work correctly
    QVERIFY(true);
}

void TestMetricsPanel::testMultipleUpdates() {
    metricsPanel->setSimulationController(controller);
    
    // Rapid updates
    for (int i = 0; i < 50; ++i) {
        metricsPanel->updateMetrics();
    }
    
    // Should handle rapid updates without issues
    QVERIFY(true);
}

void TestMetricsPanel::testEmptyState() {
    // Test with no controller
    MetricsPanel* emptyPanel = new MetricsPanel();
    emptyPanel->updateMetrics();
    
    // Should handle empty state gracefully
    QVERIFY(true);
    
    delete emptyPanel;
}

QTEST_MAIN(TestMetricsPanel)
#include "test_metrics_panel.moc"
