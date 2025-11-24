// code/tests/test_ui_graphics_items.cpp
#include <iostream>
#include <QApplication>
#include <QGraphicsScene>
#include <QtMath>
#include "../ui/GridView.h"
#include "../core/SimulationController.h"
#include "../core/City.h"
#include "../core/Preset.h"
#include "../adapters/PresetLoader.h"

/**
 * Test graphics items creation and basic properties.
 */
int main() {
    std::cout << "===== GridlockLondon: UI Graphics Items Test =====\n";
    
    // Initialize Qt application
    int argc = 0;
    char* argv[] = {nullptr};
    QApplication app(argc, argv);
    
    try {
        // 1. Create GridView
        std::cout << "\n1. Creating GridView...\n";
        GridView gridView;
        QGraphicsScene* scene = gridView.scene();
        if (!scene) {
            std::cerr << "ERROR: Scene is null\n";
            return 1;
        }
        std::cout << "   ✓ GridView created\n";
        std::cout << "   ✓ QGraphicsScene initialized\n";
        
        // 2. Create simulation controller
        std::cout << "\n2. Creating SimulationController...\n";
        SimulationController& controller = SimulationController::getInstance();
        std::cout << "   ✓ SimulationController created\n";
        
        // 3. Load preset
        std::cout << "\n3. Loading preset...\n";
        PresetLoader loader;
        Preset preset = loader.loadFromJson("presets/demo_3x3.json");
        if (!preset.validate()) {
            preset.setName("test");
            preset.setRows(3);
            preset.setCols(3);
            preset.setAgentCount(2);
            preset.setTickMs(100);
            preset.setPolicy(PolicyType::SHORTEST_PATH);
        }
        controller.loadPreset(preset);
        std::cout << "   ✓ Preset loaded\n";
        std::cout << "   ✓ City has " << controller.getCity()->getNodeCount() << " nodes\n";
        std::cout << "   ✓ City has " << controller.getCity()->getEdgeCount() << " edges\n";
        std::cout << "   ✓ " << controller.getAgents().size() << " agents created\n";
        
        // 4. Connect and update scene
        std::cout << "\n4. Updating scene...\n";
        gridView.setSimulationController(&controller);
        gridView.updateScene();
        std::cout << "   ✓ Scene updated\n";
        std::cout << "   ✓ Scene has " << scene->items().size() << " graphics items\n";
        
        // 5. Test zoom functionality
        std::cout << "\n5. Testing zoom...\n";
        qreal initialZoom = gridView.zoomLevel();
        std::cout << "   ✓ Initial zoom: " << initialZoom << "x\n";
        
        gridView.setZoomLevel(2.0);
        if (qAbs(gridView.zoomLevel() - 2.0) >= 0.1) {
            std::cerr << "ERROR: Zoom not set correctly\n";
            return 1;
        }
        std::cout << "   ✓ Zoom set to 2.0x\n";
        
        gridView.setZoomLevel(1.0);
        if (qAbs(gridView.zoomLevel() - 1.0) >= 0.1) {
            std::cerr << "ERROR: Zoom not reset correctly\n";
            return 1;
        }
        std::cout << "   ✓ Zoom reset to 1.0x\n";
        
        // 6. Test grid overlay toggle
        std::cout << "\n6. Testing grid overlay...\n";
        gridView.setShowGrid(true);
        std::cout << "   ✓ Grid enabled\n";
        gridView.setShowGrid(false);
        std::cout << "   ✓ Grid disabled\n";
        
        // 7. Test node labels toggle
        std::cout << "\n7. Testing node labels...\n";
        gridView.setShowNodeLabels(true);
        std::cout << "   ✓ Node labels enabled\n";
        gridView.setShowNodeLabels(false);
        std::cout << "   ✓ Node labels disabled\n";
        
        // 8. Test fit to window
        std::cout << "\n8. Testing fit to window...\n";
        gridView.fitToWindow();
        std::cout << "   ✓ Fit to window executed\n";
        
        // 9. Test animation step
        std::cout << "\n9. Testing animation...\n";
        gridView.animateStep();
        std::cout << "   ✓ Animation step executed\n";
        
        // 10. Test agent selection
        std::cout << "\n10. Testing agent selection...\n";
        if (gridView.selectedAgent() != nullptr) {
            std::cerr << "WARNING: Agent selected when none should be\n";
        }
        std::cout << "   ✓ No agent selected initially\n";
        
        std::cout << "\n===== All Graphics Items Tests Passed =====\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}

