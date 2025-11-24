#include "MainWindow.h"
#include "GridView.h"
#include "MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Agent.h"
#include "../core/Preset.h"
#include "../adapters/PresetLoader.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_gridView(nullptr), m_metricsPanel(nullptr),
      m_controller(nullptr), m_simulationTimer(new QTimer(this)), m_isRunning(false) {
    
    setWindowTitle("GridlockLondon - Traffic Simulation");
    resize(1400, 900);
    
    setupUI();
    setupStyles();
    connectSignals();
    
    // Create default simulation controller
    m_controller = &SimulationController::getInstance();
    
    // Load a default preset
    PresetLoader loader;
    try {
        Preset preset = loader.loadFromJson("presets/demo_3x3.json");
        m_controller->loadPreset(preset);
        m_gridView->setSimulationController(m_controller);
        m_metricsPanel->setSimulationController(m_controller);
    } catch (...) {
        // If preset loading fails, create a minimal setup
        // This is handled gracefully
    }
    
    updateControls();
}

MainWindow::~MainWindow() = default;

void MainWindow::setSimulationController(SimulationController* controller) {
    m_controller = controller;
    if (m_gridView) {
        m_gridView->setSimulationController(controller);
    }
    if (m_metricsPanel) {
        m_metricsPanel->setSimulationController(controller);
    }
}

void MainWindow::setupUI() {
    // Central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Left panel: Grid view
    m_gridView = new GridView(this);
    mainLayout->addWidget(m_gridView, 3);
    
    // Right panel: Controls and metrics
    QWidget* rightPanel = new QWidget(this);
    rightPanel->setFixedWidth(350);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    rightLayout->setSpacing(15);
    
    // Control panel
    m_controlPanel = new QWidget(this);
    m_controlPanel->setObjectName("controlPanel");
    QVBoxLayout* controlLayout = new QVBoxLayout(m_controlPanel);
    controlLayout->setSpacing(10);
    
    QLabel* controlsTitle = new QLabel("Controls", this);
    controlsTitle->setObjectName("sectionTitle");
    controlLayout->addWidget(controlsTitle);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    
    m_startButton = new QPushButton("▶ Start", this);
    m_startButton->setObjectName("primaryButton");
    m_pauseButton = new QPushButton("⏸ Pause", this);
    m_pauseButton->setObjectName("secondaryButton");
    m_resetButton = new QPushButton("↻ Reset", this);
    m_resetButton->setObjectName("secondaryButton");
    
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_pauseButton);
    buttonLayout->addWidget(m_resetButton);
    controlLayout->addLayout(buttonLayout);
    
    // View controls
    m_fitToWindowButton = new QPushButton("Fit to Window", this);
    m_fitToWindowButton->setObjectName("secondaryButton");
    controlLayout->addWidget(m_fitToWindowButton);
    
    controlLayout->addSpacing(10);
    
    m_showGridCheckbox = new QCheckBox("Show Grid", this);
    m_showGridCheckbox->setChecked(true);
    controlLayout->addWidget(m_showGridCheckbox);
    
    m_showNodeLabelsCheckbox = new QCheckBox("Show Node Labels", this);
    m_showNodeLabelsCheckbox->setChecked(false);
    controlLayout->addWidget(m_showNodeLabelsCheckbox);
    
    rightLayout->addWidget(m_controlPanel);
    
    // Metrics panel
    m_metricsPanel = new MetricsPanel(this);
    rightLayout->addWidget(m_metricsPanel, 1);
    
    // Agent info panel
    m_agentInfoPanel = new QWidget(this);
    m_agentInfoPanel->setObjectName("agentInfoPanel");
    m_agentInfoPanel->setVisible(false);
    QVBoxLayout* agentInfoLayout = new QVBoxLayout(m_agentInfoPanel);
    agentInfoLayout->setSpacing(10);
    
    m_agentInfoTitle = new QLabel("Agent Information", this);
    m_agentInfoTitle->setObjectName("sectionTitle");
    agentInfoLayout->addWidget(m_agentInfoTitle);
    
    m_agentInfoContent = new QLabel("", this);
    m_agentInfoContent->setObjectName("agentInfoContent");
    m_agentInfoContent->setWordWrap(true);
    agentInfoLayout->addWidget(m_agentInfoContent);
    
    rightLayout->addWidget(m_agentInfoPanel);
    
    mainLayout->addWidget(rightPanel);
}

void MainWindow::setupStyles() {
    // Modern dark theme stylesheet
    QString style = R"(
        QMainWindow {
            background-color: #121218;
        }
        
        QWidget {
            background-color: #1a1a24;
            color: #e0e0e8;
            font-family: 'Inter', 'SF Pro Display', -apple-system, sans-serif;
            font-size: 13px;
        }
        
        #controlPanel, #agentInfoPanel {
            background-color: #1e1e2a;
            border-radius: 12px;
            padding: 16px;
        }
        
        #sectionTitle {
            font-size: 16px;
            font-weight: 600;
            color: #ffffff;
            margin-bottom: 8px;
        }
        
        QPushButton {
            background-color: #2a2a3a;
            border: 1px solid #3a3a4a;
            border-radius: 8px;
            padding: 10px 16px;
            font-weight: 500;
            color: #e0e0e8;
            min-height: 20px;
        }
        
        QPushButton:hover {
            background-color: #3a3a4a;
            border-color: #4a4a5a;
        }
        
        QPushButton:pressed {
            background-color: #1a1a2a;
        }
        
        #primaryButton {
            background-color: #3b82f6;
            border-color: #3b82f6;
            color: #ffffff;
        }
        
        #primaryButton:hover {
            background-color: #2563eb;
        }
        
        #secondaryButton {
            background-color: #2a2a3a;
        }
        
        QCheckBox {
            spacing: 8px;
            color: #c0c0d0;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #4a4a5a;
            border-radius: 4px;
            background-color: #1a1a24;
        }
        
        QCheckBox::indicator:checked {
            background-color: #3b82f6;
            border-color: #3b82f6;
        }
        
        #agentInfoContent {
            color: #b0b0c0;
            line-height: 1.6;
            font-size: 12px;
        }
        
        #metricsPanel {
            background-color: #1e1e2a;
            border-radius: 12px;
            padding: 16px;
        }
        
        #metricLabel {
            color: #c0c0d0;
            font-size: 13px;
            padding: 4px 0;
        }
    )";
    
    setStyleSheet(style);
}

void MainWindow::connectSignals() {
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    connect(m_fitToWindowButton, &QPushButton::clicked, this, &MainWindow::onFitToWindow);
    connect(m_showGridCheckbox, &QCheckBox::toggled, this, &MainWindow::onToggleGrid);
    connect(m_showNodeLabelsCheckbox, &QCheckBox::toggled, this, &MainWindow::onToggleNodeLabels);
    
    if (m_gridView) {
        connect(m_gridView, &GridView::agentSelected, this, &MainWindow::onAgentSelected);
    }
    
    // Simulation timer
    connect(m_simulationTimer, &QTimer::timeout, this, &MainWindow::onTick);
    m_simulationTimer->setInterval(100); // 100ms per tick
}

void MainWindow::updateControls() {
    m_startButton->setEnabled(!m_isRunning);
    m_pauseButton->setEnabled(m_isRunning);
}

void MainWindow::onStartClicked() {
    if (!m_controller) return;
    
    m_controller->start();
    m_isRunning = true;
    m_simulationTimer->start();
    updateControls();
}

void MainWindow::onPauseClicked() {
    if (!m_controller) return;
    
    m_controller->pause();
    m_isRunning = false;
    m_simulationTimer->stop();
    updateControls();
}

void MainWindow::onResetClicked() {
    if (!m_controller) return;
    
    m_controller->reset();
    m_isRunning = false;
    m_simulationTimer->stop();
    
    if (m_gridView) {
        m_gridView->updateScene();
    }
    if (m_metricsPanel) {
        m_metricsPanel->updateMetrics();
    }
    
    updateControls();
}

void MainWindow::onTick() {
    if (!m_controller || !m_isRunning) return;
    
    m_controller->tick();
    
    if (m_gridView) {
        m_gridView->animateStep();
    }
    if (m_metricsPanel) {
        m_metricsPanel->updateMetrics();
    }
}

void MainWindow::onAgentSelected(Agent* agent) {
    if (!agent || !m_controller) {
        m_agentInfoPanel->setVisible(false);
        return;
    }
    
    // Verify agent is still valid
    bool isValid = false;
    auto& agents = m_controller->getAgents();
    for (Agent* a : agents) {
        if (a == agent) {
            isValid = true;
            break;
        }
    }
    
    if (!isValid) {
        m_agentInfoPanel->setVisible(false);
        return;
    }
    
    m_agentInfoPanel->setVisible(true);
    
    // Get policy name
    QString policyName = (m_controller->getPolicy() == PolicyType::CONGESTION_AWARE) 
                         ? "Congestion-Aware" 
                         : "Shortest Path";
    
    QString info = QString(
        "<b>Agent %1</b><br><br>"
        "Origin: Node %2<br>"
        "Destination: Node %3<br>"
        "Current: Node %4<br>"
        "Policy: %5<br>"
        "Time elapsed: %6 ticks<br>"
        "Status: %7"
    ).arg(agent->getId())
     .arg(agent->getOrigin())
     .arg(agent->getDestination())
     .arg(agent->getCurrentNode())
     .arg(policyName)
     .arg(agent->getTravelTime())
     .arg(agent->hasArrived() ? "Arrived" : "In transit");
    
    m_agentInfoContent->setText(info);
}

void MainWindow::onFitToWindow() {
    if (m_gridView) {
        m_gridView->fitToWindow();
    }
}

void MainWindow::onToggleGrid() {
    if (m_gridView) {
        m_gridView->setShowGrid(m_showGridCheckbox->isChecked());
    }
}

void MainWindow::onToggleNodeLabels() {
    if (m_gridView) {
        m_gridView->setShowNodeLabels(m_showNodeLabelsCheckbox->isChecked());
    }
}
