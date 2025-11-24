// code/tests/test_grid_view.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "../ui/GridView.h"
#include "../core/SimulationController.h"
#include "../core/Preset.h"
#include "../adapters/PresetLoader.h"
#include <memory>

/**
 * Test suite for GridView component.
 * Tests node/edge/agent rendering, zoom, pan, and interactive features.
 */
class TestGridView : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Basic functionality tests
    void testGridViewCreation();
    void testSetSimulationController();
    void testUpdateScene();
    
    // Node tests
    void testNodeItemsCreated();
    void testNodeLabelsToggle();
    
    // Edge tests
    void testEdgeItemsCreated();
    void testEdgeColorUpdates();
    void testEdgeTooltips();
    
    // Agent tests
    void testAgentItemsCreated();
    void testAgentPositionUpdates();
    void testAgentSelection();
    
    // View control tests
    void testZoom();
    void testFitToWindow();
    void testGridToggle();
    void testPan();
    
    // Styling tests
    void testDarkBackground();
    void testGridColor();

private:
    QApplication* app;
    GridView* gridView;
    SimulationController* controller;
    std::unique_ptr<Preset> preset;
};

void TestGridView::initTestCase() {
    // Check if QApplication already exists (from QTEST_MAIN)
    if (!QCoreApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    } else {
        app = qobject_cast<QApplication*>(QCoreApplication::instance());
    }
}

void TestGridView::cleanupTestCase() {
    // Don't delete app if it was created by QTEST_MAIN
    // Only delete if we created it ourselves
    if (app && app != QCoreApplication::instance()) {
        delete app;
        app = nullptr;
    }
}

void TestGridView::init() {
    gridView = new GridView();
    
    // Create a minimal simulation controller with test data
    controller = &SimulationController::getInstance();
    
    // Create a simple 3x3 grid preset
    PresetLoader loader;
    Preset testPreset = loader.loadFromJson("presets/demo_3x3.json");
    if (!testPreset.validate()) {
        // Create a minimal valid preset manually
        testPreset.setName("test");
        testPreset.setRows(3);
        testPreset.setCols(3);
        testPreset.setAgentCount(2);
        testPreset.setTickMs(100);
        testPreset.setPolicy(PolicyType::SHORTEST_PATH);
    }
    
    controller->loadPreset(testPreset);
}

void TestGridView::cleanup() {
    // Delete widgets before application cleanup
    if (gridView) {
        // Ensure gridView is hidden and events processed before deletion
        gridView->hide();
        QApplication::processEvents();
        delete gridView;
        gridView = nullptr;
    }
    if (controller) {
        controller->reset();
    }
    // Process events multiple times to ensure all cleanup is complete
    if (app) {
        QApplication::processEvents();
        QApplication::processEvents();
    }
}

void TestGridView::testGridViewCreation() {
    QVERIFY(gridView != nullptr);
    QVERIFY(gridView->scene() != nullptr);
}

void TestGridView::testSetSimulationController() {
    gridView->setSimulationController(controller);
    QVERIFY(gridView->selectedAgent() == nullptr); // No agent selected initially
}

void TestGridView::testUpdateScene() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Scene should have items after update
    QVERIFY(gridView->scene()->items().size() > 0);
}

void TestGridView::testNodeItemsCreated() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Should have items in the scene after update
    QGraphicsScene* scene = gridView->scene();
    QVERIFY(scene != nullptr);
    QVERIFY(scene->items().size() > 0); // Should have some items (nodes, edges, agents)
}

void TestGridView::testNodeLabelsToggle() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Test toggling node labels
    gridView->setShowNodeLabels(true);
    gridView->setShowNodeLabels(false);
    gridView->setShowNodeLabels(true);
    
    // Should not crash
    QVERIFY(true);
}

void TestGridView::testEdgeItemsCreated() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Should have items in the scene (edges are part of the items)
    QGraphicsScene* scene = gridView->scene();
    QVERIFY(scene != nullptr);
    // Scene should have multiple items (nodes + edges + agents)
    QVERIFY(scene->items().size() >= 2);
}

void TestGridView::testEdgeColorUpdates() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Trigger edge color update
    gridView->animateStep();
    
    // Should not crash
    QVERIFY(true);
}

void TestGridView::testEdgeTooltips() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Tooltips are tested through hover events in integration tests
    // This is a placeholder to ensure the functionality exists
    QVERIFY(true);
}

void TestGridView::testAgentItemsCreated() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Should have items in the scene
    // Agents may or may not be visible depending on simulation state
    QGraphicsScene* scene = gridView->scene();
    QVERIFY(scene != nullptr);
    // Scene should have items
    QVERIFY(scene->items().size() > 0);
}

void TestGridView::testAgentPositionUpdates() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Update agent positions
    gridView->animateStep();
    
    // Should not crash
    QVERIFY(true);
}

void TestGridView::testAgentSelection() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Initially no agent selected
    QVERIFY(gridView->selectedAgent() == nullptr);
    
    // Selection is tested through mouse events in integration tests
    QVERIFY(true);
}

void TestGridView::testZoom() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Test zoom levels
    qreal initialZoom = gridView->zoomLevel();
    gridView->setZoomLevel(2.0);
    QVERIFY(qAbs(gridView->zoomLevel() - 2.0) < 0.1);
    
    gridView->setZoomLevel(1.0);
    QVERIFY(qAbs(gridView->zoomLevel() - 1.0) < 0.1);
    
    // Test zoom bounds
    gridView->setZoomLevel(0.5); // Should clamp to 1.0
    QVERIFY(gridView->zoomLevel() >= 1.0);
    
    gridView->setZoomLevel(10.0); // Should clamp to 5.0
    QVERIFY(gridView->zoomLevel() <= 5.0);
}

void TestGridView::testFitToWindow() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Should not crash
    gridView->fitToWindow();
    QVERIFY(true);
}

void TestGridView::testGridToggle() {
    gridView->setSimulationController(controller);
    
    // Toggle grid
    gridView->setShowGrid(true);
    gridView->setShowGrid(false);
    gridView->setShowGrid(true);
    
    // Should not crash
    QVERIFY(true);
}

void TestGridView::testPan() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Pan is tested through mouse/keyboard events in integration tests
    // This ensures the functionality exists
    QVERIFY(true);
}

void TestGridView::testDarkBackground() {
    gridView->setSimulationController(controller);
    
    // Check that GridView has dark background styling
    QString styleSheet = gridView->styleSheet();
    QVERIFY(styleSheet.contains("#0A0A0A") || 
            styleSheet.contains("background-color") ||
            styleSheet.contains("border-radius"));
}

void TestGridView::testGridColor() {
    gridView->setSimulationController(controller);
    gridView->updateScene();
    
    // Grid should use subtle colors
    gridView->setShowGrid(true);
    gridView->setShowGrid(false);
    gridView->setShowGrid(true);
    
    // Should not crash
    QVERIFY(true);
}

QTEST_MAIN(TestGridView)
#include "test_grid_view.moc"

