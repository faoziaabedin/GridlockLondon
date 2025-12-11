#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QComboBox>
#include <QSlider>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QGroupBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <memory>

class GridView;
class MetricsPanel;
class SimulationController;
class Agent;

/**
 * Main window with modern dark theme and professional UI.
 * Features: Toolbar, collapsible control panel, metrics panel, status bar, menu bar.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void setSimulationController(SimulationController* controller);

private slots:
    void onStartClicked();
    void onPauseClicked();
    void onResetClicked();
    void onTick();
    void onAgentSelected(Agent* agent);
    void onFitToWindow();
    void onToggleGrid();
    void onToggleNodeLabels();
    void onToggleAgentTrails();
    void onToggleHeatMap();
    void onPolicyChanged(int index);
    void onSpeedChanged(int value);
    void onLoadPreset();
    void onExportCSV();
    void onExportJSON();
    void onSaveConfig();
    void onZoomIn();
    void onZoomOut();
    void onAbout();
    void onShortcuts();
    void onSectionToggled();
    void onOpenAnalytics();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupControlPanel();
    void setupStyles();
    void connectSignals();
    void updateControls();
    void updateToolbarStats();
    void showToast(const QString& message);
    void updateFPS();
    
    // UI Components
    GridView* m_gridView;
    MetricsPanel* m_metricsPanel;
    QWidget* m_centralWidget;
    QWidget* m_controlPanel;
    QWidget* m_agentInfoPanel;
    class AnalyticsPanel* m_analyticsPanel;  // Forward declaration
    
    // Toolbar
    QToolBar* m_toolbar;
    QPushButton* m_startButton;
    QPushButton* m_pauseButton;
    QPushButton* m_resetButton;
    QComboBox* m_policyCombo;
    QSlider* m_speedSlider;
    QLabel* m_speedLabel;
    QLabel* m_tickLabel;
    QLabel* m_agentsLabel;
    
    // Control Panel
    QGroupBox* m_simulationGroup;
    QGroupBox* m_settingsGroup;
    QGroupBox* m_visualizationGroup;
    QGroupBox* m_quickStatsGroup;
    QComboBox* m_presetCombo;
    QPushButton* m_loadPresetButton;
    QSpinBox* m_gridSizeRows;
    QSpinBox* m_gridSizeCols;
    QSpinBox* m_agentsSpin;
    QSpinBox* m_tickIntervalSpin;
    QCheckBox* m_showGridCheckbox;
    QCheckBox* m_showNodeLabelsCheckbox;
    QCheckBox* m_agentTrailsCheckbox;
    QCheckBox* m_heatMapCheckbox;
    QLabel* m_avgTripLabel;
    QLabel* m_throughputLabel;
    QLabel* m_maxLoadLabel;
    
    // Status Bar
    QLabel* m_statusIndicator;
    QLabel* m_fpsLabel;
    QLabel* m_updateTimeLabel;
    
    // Zoom Bar
    QSlider* m_zoomSlider;
    QLabel* m_zoomLabel;
    
    // Menu Bar
    QMenu* m_fileMenu;
    QMenu* m_simulationMenu;
    QMenu* m_viewMenu;
    QMenu* m_helpMenu;
    
    // Toast notification
    QLabel* m_toastLabel;
    QPropertyAnimation* m_toastAnimation;
    
    // Traffic Legend Labels (dynamic based on grid)
    QLabel* m_legendEmpty;
    QLabel* m_legendLight;
    QLabel* m_legendModerate;
    QLabel* m_legendHeavy;
    QLabel* m_legendGridlock;
    int m_currentCapacity;
    void updateTrafficLegend(int capacity);
    
    // Simulation
    SimulationController* m_controller;
    QTimer* m_simulationTimer;
    QTimer* m_fpsTimer;
    bool m_isRunning;
    int m_currentSpeed; // 1-10x multiplier
    int m_frameCount;
    qint64 m_lastFpsUpdate;
};
