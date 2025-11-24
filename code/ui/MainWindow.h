#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <memory>

class GridView;
class MetricsPanel;
class SimulationController;
class Agent;

/**
 * Main window with modern dark theme and professional UI.
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

private:
    void setupUI();
    void setupStyles();
    void connectSignals();
    void updateControls();

    // UI Components
    GridView* m_gridView;
    MetricsPanel* m_metricsPanel;
    QWidget* m_centralWidget;
    QWidget* m_controlPanel;
    QWidget* m_agentInfoPanel;
    
    // Controls
    QPushButton* m_startButton;
    QPushButton* m_pauseButton;
    QPushButton* m_resetButton;
    QPushButton* m_fitToWindowButton;
    QCheckBox* m_showGridCheckbox;
    QCheckBox* m_showNodeLabelsCheckbox;
    
    // Agent info
    QLabel* m_agentInfoTitle;
    QLabel* m_agentInfoContent;
    
    // Simulation
    SimulationController* m_controller;
    QTimer* m_simulationTimer;
    bool m_isRunning;
};
