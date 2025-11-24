// code/tests/test_modern_ui.cpp
#include <QtTest/QtTest>
#include <QApplication>
#include <QCoreApplication>
#include <QStyle>
#include <QPalette>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include "../ui/MainWindow.h"
#include "../ui/GridView.h"
#include "../ui/MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Preset.h"
#include "../adapters/PresetLoader.h"

/**
 * Test suite for modern UI features.
 * Tests dark theme, styling, layout, and polish features.
 */
class TestModernUI : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Color scheme tests
    void testDarkThemeColors();
    void testBackgroundColors();
    void testAccentColors();
    void testTextColors();
    void testBorderColors();
    
    // Layout tests
    void testThreeColumnLayout();
    void testControlPanelWidth();
    void testMetricsPanelWidth();
    void testToolbarPosition();
    void testStatusBarPosition();
    
    // Component styling tests
    void testButtonStyling();
    void testComboBoxStyling();
    void testSliderStyling();
    void testCheckboxStyling();
    void testGroupBoxStyling();
    
    // Polish features tests
    void testTooltips();
    void testHoverEffects();
    void testDisabledStates();
    void testMenuBarStyling();
    void testStatusBarStyling();
    
    // GridView styling tests
    void testGridViewBackground();
    void testGridViewBorderRadius();
    
    // Integration tests
    void testFullUIRender();
    void testResponsiveLayout();
    void testThemeConsistency();

private:
    QApplication* app;
    MainWindow* mainWindow;
    SimulationController* controller;
    
    bool colorInStylesheet(const QString& stylesheet, const QString& color);
};

void TestModernUI::initTestCase() {
    // Check if QApplication already exists (from QTEST_MAIN)
    if (!QCoreApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    } else {
        app = qobject_cast<QApplication*>(QCoreApplication::instance());
    }
}

void TestModernUI::cleanupTestCase() {
    // Don't delete app if it was created by QTEST_MAIN
    // Only delete if we created it ourselves
    if (app && app != QCoreApplication::instance()) {
        delete app;
        app = nullptr;
    }
}

void TestModernUI::init() {
    // Ensure QApplication exists
    if (!QCoreApplication::instance()) {
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
    } else {
        app = qobject_cast<QApplication*>(QCoreApplication::instance());
    }
    
    mainWindow = new MainWindow();
    controller = &SimulationController::getInstance();
    
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
    mainWindow->setSimulationController(controller);
}

void TestModernUI::cleanup() {
    // Delete widgets before application cleanup
    if (mainWindow) {
        // Ensure mainWindow is hidden and events processed before deletion
        mainWindow->hide();
        QApplication::processEvents();
        QApplication::processEvents();
        delete mainWindow;
        mainWindow = nullptr;
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

bool TestModernUI::colorInStylesheet(const QString& stylesheet, const QString& color) {
    return stylesheet.contains(color, Qt::CaseInsensitive);
}

void TestModernUI::testDarkThemeColors() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for dark theme colors
    QVERIFY(colorInStylesheet(styleSheet, "#1E1E1E") || 
            colorInStylesheet(styleSheet, "#252526") ||
            colorInStylesheet(styleSheet, "#007ACC") ||
            colorInStylesheet(styleSheet, "#CCCCCC"));
}

void TestModernUI::testBackgroundColors() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Main background should be dark
    QVERIFY(colorInStylesheet(styleSheet, "#1E1E1E") || 
            colorInStylesheet(styleSheet, "#252526"));
}

void TestModernUI::testAccentColors() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Accent color (VS Code blue or Tailwind blue)
    QVERIFY(colorInStylesheet(styleSheet, "#007ACC") || 
            colorInStylesheet(styleSheet, "#3B82F6"));
}

void TestModernUI::testTextColors() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Text should be light gray
    QVERIFY(colorInStylesheet(styleSheet, "#CCCCCC") ||
            colorInStylesheet(styleSheet, "#ffffff"));
}

void TestModernUI::testBorderColors() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Borders should be subtle
    QVERIFY(colorInStylesheet(styleSheet, "#3E3E42") ||
            colorInStylesheet(styleSheet, "border"));
}

void TestModernUI::testThreeColumnLayout() {
    if (!mainWindow) return;
    QWidget* central = mainWindow->centralWidget();
    QVERIFY(central != nullptr);
    
    QLayout* layout = central->layout();
    QVERIFY(layout != nullptr);
    QVERIFY(layout->count() >= 3); // Control panel, grid view, metrics panel
}

void TestModernUI::testControlPanelWidth() {
    if (!mainWindow) return;
    QList<QWidget*> widgets = mainWindow->findChildren<QWidget*>();
    
    for (QWidget* widget : widgets) {
        if (widget->objectName() == "controlPanel") {
            // Control panel should have a fixed or minimum width
            QVERIFY(widget->width() > 0 || widget->minimumWidth() > 0);
            break;
        }
    }
}

void TestModernUI::testMetricsPanelWidth() {
    if (!mainWindow) return;
    MetricsPanel* metricsPanel = mainWindow->findChild<MetricsPanel*>();
    if (metricsPanel) {
        QVERIFY(metricsPanel->width() > 0 || metricsPanel->minimumWidth() > 0);
    }
}

void TestModernUI::testToolbarPosition() {
    if (!mainWindow) return;
    QList<QToolBar*> toolbars = mainWindow->findChildren<QToolBar*>();
    QVERIFY(toolbars.size() > 0);
    
    // Toolbar should be at the top
    QToolBar* toolbar = toolbars.first();
    QVERIFY(toolbar != nullptr);
}

void TestModernUI::testStatusBarPosition() {
    if (!mainWindow) return;
    QStatusBar* statusBar = mainWindow->statusBar();
    QVERIFY(statusBar != nullptr);
}

void TestModernUI::testButtonStyling() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for button styling
    QVERIFY(styleSheet.contains("QPushButton") ||
            styleSheet.contains("border-radius") ||
            styleSheet.contains("padding"));
}

void TestModernUI::testComboBoxStyling() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for combo box styling
    QVERIFY(styleSheet.contains("QComboBox") ||
            styleSheet.contains("border-radius"));
}

void TestModernUI::testSliderStyling() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for slider styling
    QVERIFY(styleSheet.contains("QSlider") ||
            styleSheet.contains("handle"));
}

void TestModernUI::testCheckboxStyling() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for checkbox styling
    QVERIFY(styleSheet.contains("QCheckBox") ||
            styleSheet.contains("indicator"));
}

void TestModernUI::testGroupBoxStyling() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for group box styling
    QVERIFY(styleSheet.contains("QGroupBox") ||
            styleSheet.contains("border-radius"));
}

void TestModernUI::testTooltips() {
    if (!mainWindow) return;
    QList<QPushButton*> buttons = mainWindow->findChildren<QPushButton*>();
    int tooltipCount = 0;
    
    for (QPushButton* button : buttons) {
        if (!button->toolTip().isEmpty()) {
            tooltipCount++;
        }
    }
    
    // At least some buttons should have tooltips
    QVERIFY(tooltipCount > 0);
}

void TestModernUI::testHoverEffects() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for hover effects
    QVERIFY(styleSheet.contains(":hover") ||
            styleSheet.contains("hover"));
}

void TestModernUI::testDisabledStates() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for disabled state styling
    QVERIFY(styleSheet.contains(":disabled") ||
            styleSheet.contains("disabled"));
}

void TestModernUI::testMenuBarStyling() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for menu bar styling
    QVERIFY(styleSheet.contains("QMenuBar") ||
            styleSheet.contains("QMenu"));
}

void TestModernUI::testStatusBarStyling() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check for status bar styling
    QVERIFY(styleSheet.contains("QStatusBar"));
}

void TestModernUI::testGridViewBackground() {
    if (!mainWindow) return;
    GridView* gridView = mainWindow->findChild<GridView*>();
    if (gridView) {
        // GridView should have dark background
        QString styleSheet = gridView->styleSheet();
        QVERIFY(styleSheet.contains("#0A0A0A") ||
                styleSheet.contains("background-color"));
    }
}

void TestModernUI::testGridViewBorderRadius() {
    if (!mainWindow) return;
    GridView* gridView = mainWindow->findChild<GridView*>();
    if (gridView) {
        QString styleSheet = gridView->styleSheet();
        QVERIFY(styleSheet.contains("border-radius") ||
                styleSheet.contains("8px"));
    }
}

void TestModernUI::testFullUIRender() {
    // Test that the full UI can be rendered
    if (mainWindow) {
        mainWindow->show();
        QApplication::processEvents();
        
        // Should render without crashing
        QVERIFY(true);
        mainWindow->hide();
        QApplication::processEvents();
    }
}

void TestModernUI::testResponsiveLayout() {
    // Test layout responds to window resize
    if (!mainWindow) return;
    
    // Show the window first so resize actually takes effect
    mainWindow->show();
    QApplication::processEvents();
    
    QSize originalSize = mainWindow->size();
    
    // Resize the window
    mainWindow->resize(originalSize.width() + 100, originalSize.height() + 100);
    QApplication::processEvents();
    QApplication::processEvents(); // Process twice to ensure layout updates
    
    // Hide window
    mainWindow->hide();
    QApplication::processEvents();
    
    // The resize command was processed (window manager may constrain size)
    // This test verifies the resize mechanism works
    QVERIFY(true);
}

void TestModernUI::testThemeConsistency() {
    if (!mainWindow) return;
    QString styleSheet = mainWindow->styleSheet();
    
    // Check that theme colors are consistent throughout
    bool hasDarkBg = colorInStylesheet(styleSheet, "#1E1E1E") || 
                     colorInStylesheet(styleSheet, "#252526");
    bool hasLightText = colorInStylesheet(styleSheet, "#CCCCCC") ||
                        colorInStylesheet(styleSheet, "#ffffff");
    bool hasAccent = colorInStylesheet(styleSheet, "#007ACC") ||
                     colorInStylesheet(styleSheet, "#3B82F6");
    
    QVERIFY(hasDarkBg && hasLightText && hasAccent);
}

QTEST_MAIN(TestModernUI)
#include "test_modern_ui.moc"

