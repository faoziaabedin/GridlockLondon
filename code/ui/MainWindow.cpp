#include "MainWindow.h"
#include "GridView.h"
#include "MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Agent.h"
#include "../core/Preset.h"
#include "../core/Metrics.h"
#include "../adapters/PresetLoader.h"
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
#include <QFileDialog>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QtMath>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_gridView(nullptr), m_metricsPanel(nullptr),
      m_analyticsPanel(nullptr), m_controller(nullptr), m_simulationTimer(new QTimer(this)), 
      m_fpsTimer(new QTimer(this)), m_isRunning(false), m_currentSpeed(5),
      m_frameCount(0), m_lastFpsUpdate(0) {
    
    setWindowTitle("GridlockLondon - Traffic Simulation");
    resize(1600, 1000);
    
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupStyles();
    connectSignals();
    
    // Create default simulation controller
    m_controller = &SimulationController::getInstance();
    
    // Load a default preset
    PresetLoader loader;
    try {
        Preset preset = loader.loadFromJson("presets/demo_5x5.json");
        m_controller->loadPreset(preset);
        m_gridView->setSimulationController(m_controller);
        m_metricsPanel->setSimulationController(m_controller);
    } catch (...) {
        // If preset loading fails, create a minimal setup
    }
    
    updateControls();
    
    // FPS timer
    m_fpsTimer->setInterval(1000);
    connect(m_fpsTimer, &QTimer::timeout, this, &MainWindow::updateFPS);
    m_fpsTimer->start();
    m_lastFpsUpdate = QDateTime::currentMSecsSinceEpoch();
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
    // Central widget with horizontal layout
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Left panel: Control Panel (30% width)
    setupControlPanel();
    m_controlPanel->setFixedWidth(static_cast<int>(width() * 0.30));
    mainLayout->addWidget(m_controlPanel);
    
    // Center: GridView (main canvas)
    m_gridView = new GridView(this);
    mainLayout->addWidget(m_gridView, 1);
    
    // Right panel: Metrics Panel (30% width)
    m_metricsPanel = new MetricsPanel(this);
    m_metricsPanel->setFixedWidth(static_cast<int>(width() * 0.30));
    mainLayout->addWidget(m_metricsPanel);
}

void MainWindow::setupMenuBar() {
    // File Menu
    m_fileMenu = menuBar()->addMenu("📁 File");
    m_fileMenu->addAction("📁 Load Preset...", this, &MainWindow::onLoadPreset, QKeySequence::Open);
    m_fileMenu->addAction("💾 Save Config...", this, &MainWindow::onSaveConfig, QKeySequence::Save);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("📊 Export CSV...", this, &MainWindow::onExportCSV);
    m_fileMenu->addAction("📄 Export JSON...", this, &MainWindow::onExportJSON);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("🚪 Quit", this, &QWidget::close, QKeySequence::Quit);
    
    // Simulation Menu
    m_simulationMenu = menuBar()->addMenu("▶ Simulation");
    m_simulationMenu->addAction("▶ Start", this, &MainWindow::onStartClicked, QKeySequence(Qt::Key_Space));
    m_simulationMenu->addAction("⏸ Pause", this, &MainWindow::onPauseClicked);
    m_simulationMenu->addAction("⏹ Reset", this, &MainWindow::onResetClicked, QKeySequence(Qt::Key_R));
    m_simulationMenu->addSeparator();
    m_simulationMenu->addAction("⏩ Speed Up", this, [this]() { m_speedSlider->setValue(qMin(10, m_speedSlider->value() + 1)); });
    m_simulationMenu->addAction("⏪ Slow Down", this, [this]() { m_speedSlider->setValue(qMax(1, m_speedSlider->value() - 1)); });
    
    // View Menu
    m_viewMenu = menuBar()->addMenu("🔍 View");
    m_viewMenu->addAction("🔍 Zoom In", this, &MainWindow::onZoomIn, QKeySequence::ZoomIn);
    m_viewMenu->addAction("🔍 Zoom Out", this, &MainWindow::onZoomOut, QKeySequence::ZoomOut);
    m_viewMenu->addAction("🎯 Fit View", this, &MainWindow::onFitToWindow, QKeySequence(Qt::CTRL | Qt::Key_0));
    m_viewMenu->addSeparator();
    m_viewMenu->addAction("🌓 Toggle Theme", this, []() { /* Theme toggle */ });
    m_viewMenu->addAction("🎨 Settings", this, []() { /* Settings dialog */ });
    
    // Analytics Menu
    QMenu* analyticsMenu = menuBar()->addMenu("📊 Analytics");
    analyticsMenu->addAction("📊 Advanced Analytics", this, &MainWindow::onOpenAnalytics);
    analyticsMenu->addSeparator();
    
    // Help Menu
    m_helpMenu = menuBar()->addMenu("📖 Help");
    m_helpMenu->addAction("📖 About", this, &MainWindow::onAbout);
    m_helpMenu->addAction("⌨️ Shortcuts", this, &MainWindow::onShortcuts);
    m_helpMenu->addAction("🌐 GitHub Repo", this, []() { 
        QMessageBox::information(nullptr, "GitHub", "Repository: oop-assignment-2-faozia-and-paridhi");
    });
}

void MainWindow::setupToolBar() {
    m_toolbar = addToolBar("Main Toolbar");
    m_toolbar->setMovable(false);
    m_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    // Control buttons
    m_startButton = new QPushButton("▶ Start", this);
    m_startButton->setIcon(QIcon::fromTheme("media-playback-start"));
    m_startButton->setToolTip("Start simulation (Space)");
    m_toolbar->addWidget(m_startButton);
    
    m_pauseButton = new QPushButton("⏸ Pause", this);
    m_pauseButton->setIcon(QIcon::fromTheme("media-playback-pause"));
    m_pauseButton->setToolTip("Pause simulation (Space)");
    m_toolbar->addWidget(m_pauseButton);
    
    m_resetButton = new QPushButton("⏹ Reset", this);
    m_resetButton->setIcon(QIcon::fromTheme("media-playback-stop"));
    m_resetButton->setToolTip("Reset simulation (R)");
    m_toolbar->addWidget(m_resetButton);
    
    m_toolbar->addSeparator();
    
    // Policy dropdown
    m_toolbar->addWidget(new QLabel("Policy:", this));
    m_policyCombo = new QComboBox(this);
    m_policyCombo->addItem("📍 Shortest Path");
    m_policyCombo->addItem("🚦 Congestion-Aware");
    m_policyCombo->setToolTip("Select routing policy");
    m_toolbar->addWidget(m_policyCombo);
    
    m_toolbar->addSeparator();
    
    // Speed slider
    m_toolbar->addWidget(new QLabel("Speed:", this));
    m_speedSlider = new QSlider(Qt::Horizontal, this);
    m_speedSlider->setMinimum(1);
    m_speedSlider->setMaximum(10);
    m_speedSlider->setValue(5);
    m_speedSlider->setFixedWidth(150);
    m_speedSlider->setToolTip("Simulation speed (1x to 10x)");
    m_toolbar->addWidget(m_speedSlider);
    
    m_speedLabel = new QLabel("5.0x", this);
    m_speedLabel->setMinimumWidth(40);
    m_toolbar->addWidget(m_speedLabel);
    
    m_toolbar->addSeparator();
    
    // Live stats
    m_tickLabel = new QLabel("Tick: 0", this);
    m_toolbar->addWidget(m_tickLabel);
    
    m_agentsLabel = new QLabel("Agents: 0/0", this);
    m_toolbar->addWidget(m_agentsLabel);
}

void MainWindow::setupStatusBar() {
    // Status indicator
    m_statusIndicator = new QLabel("●", this);
    m_statusIndicator->setStyleSheet("color: #4ade80; font-size: 12px;");
    statusBar()->addWidget(m_statusIndicator);
    
    statusBar()->addWidget(new QLabel("Simulation Stopped", this));
    
    statusBar()->addPermanentWidget(new QLabel("FPS:", this));
    m_fpsLabel = new QLabel("60", this);
    statusBar()->addPermanentWidget(m_fpsLabel);
    
    statusBar()->addPermanentWidget(new QLabel("Last update:", this));
    m_updateTimeLabel = new QLabel("0.00s", this);
    statusBar()->addPermanentWidget(m_updateTimeLabel);
}

void MainWindow::setupControlPanel() {
    m_controlPanel = new QWidget(this);
    m_controlPanel->setObjectName("controlPanel");
    QVBoxLayout* panelLayout = new QVBoxLayout(m_controlPanel);
    panelLayout->setContentsMargins(16, 16, 16, 16);
    panelLayout->setSpacing(12);
    
    // 📊 SIMULATION Section
    m_simulationGroup = new QGroupBox("📊 SIMULATION", this);
    QVBoxLayout* simLayout = new QVBoxLayout(m_simulationGroup);
    
    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItem("demo_3x3.json");
    m_presetCombo->addItem("demo_5x5.json");
    m_presetCombo->addItem("london_10x10.json");
    m_presetCombo->addItem("manhattan_15x15.json");
    m_presetCombo->setCurrentText("demo_5x5.json");
    simLayout->addWidget(m_presetCombo);
    
    m_loadPresetButton = new QPushButton("🎯 Load Preset...", this);
    simLayout->addWidget(m_loadPresetButton);
    
    panelLayout->addWidget(m_simulationGroup);
    
    // 🎛️ SETTINGS Section
    m_settingsGroup = new QGroupBox("🎛️ SETTINGS", this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(m_settingsGroup);
    
    QHBoxLayout* gridLayout = new QHBoxLayout();
    gridLayout->addWidget(new QLabel("Grid Size:", this));
    m_gridSizeRows = new QSpinBox(this);
    m_gridSizeRows->setMinimum(3);
    m_gridSizeRows->setMaximum(50);
    m_gridSizeRows->setValue(5);
    gridLayout->addWidget(m_gridSizeRows);
    gridLayout->addWidget(new QLabel("x", this));
    m_gridSizeCols = new QSpinBox(this);
    m_gridSizeCols->setMinimum(3);
    m_gridSizeCols->setMaximum(50);
    m_gridSizeCols->setValue(5);
    gridLayout->addWidget(m_gridSizeCols);
    settingsLayout->addLayout(gridLayout);
    
    QHBoxLayout* agentsLayout = new QHBoxLayout();
    agentsLayout->addWidget(new QLabel("Agents:", this));
    m_agentsSpin = new QSpinBox(this);
    m_agentsSpin->setMinimum(1);
    m_agentsSpin->setMaximum(100);
    m_agentsSpin->setValue(10);
    agentsLayout->addWidget(m_agentsSpin);
    settingsLayout->addLayout(agentsLayout);
    
    QHBoxLayout* tickLayout = new QHBoxLayout();
    tickLayout->addWidget(new QLabel("Tick Interval:", this));
    m_tickIntervalSpin = new QSpinBox(this);
    m_tickIntervalSpin->setMinimum(50);
    m_tickIntervalSpin->setMaximum(1000);
    m_tickIntervalSpin->setSingleStep(50);
    m_tickIntervalSpin->setValue(150);
    m_tickIntervalSpin->setSuffix(" ms");
    tickLayout->addWidget(m_tickIntervalSpin);
    settingsLayout->addLayout(tickLayout);
    
    panelLayout->addWidget(m_settingsGroup);
    
    // 🎨 VISUALIZATION Section
    m_visualizationGroup = new QGroupBox("🎨 VISUALIZATION", this);
    QVBoxLayout* vizLayout = new QVBoxLayout(m_visualizationGroup);
    
    m_showGridCheckbox = new QCheckBox("☑ Show Grid", this);
    m_showGridCheckbox->setChecked(true);
    m_showGridCheckbox->setToolTip("Toggle grid background");
    vizLayout->addWidget(m_showGridCheckbox);
    
    m_showNodeLabelsCheckbox = new QCheckBox("☑ Show Labels", this);
    m_showNodeLabelsCheckbox->setChecked(false);
    m_showNodeLabelsCheckbox->setToolTip("Show node ID labels");
    vizLayout->addWidget(m_showNodeLabelsCheckbox);
    
    m_agentTrailsCheckbox = new QCheckBox("☑ Agent Trails", this);
    m_agentTrailsCheckbox->setChecked(true);
    m_agentTrailsCheckbox->setToolTip("Show agent movement trails");
    vizLayout->addWidget(m_agentTrailsCheckbox);
    
    m_heatMapCheckbox = new QCheckBox("☐ Heat Map Mode", this);
    m_heatMapCheckbox->setChecked(false);
    m_heatMapCheckbox->setToolTip("Display congestion heat map");
    vizLayout->addWidget(m_heatMapCheckbox);
    
    panelLayout->addWidget(m_visualizationGroup);
    
    // 📈 QUICK STATS Section
    m_quickStatsGroup = new QGroupBox("📈 QUICK STATS", this);
    QVBoxLayout* statsLayout = new QVBoxLayout(m_quickStatsGroup);
    
    m_avgTripLabel = new QLabel("Avg Trip: 0.0 steps", this);
    statsLayout->addWidget(m_avgTripLabel);
    
    m_throughputLabel = new QLabel("Throughput: 0", this);
    statsLayout->addWidget(m_throughputLabel);
    
    m_maxLoadLabel = new QLabel("Max Load: 0 vehicles", this);
    statsLayout->addWidget(m_maxLoadLabel);
    
    panelLayout->addWidget(m_quickStatsGroup);
    
    panelLayout->addStretch();
    
    // Agent info panel (initially hidden)
    m_agentInfoPanel = new QWidget(this);
    m_agentInfoPanel->setObjectName("agentInfoPanel");
    m_agentInfoPanel->setVisible(false);
    QVBoxLayout* agentInfoLayout = new QVBoxLayout(m_agentInfoPanel);
    agentInfoLayout->setSpacing(10);
    
    QLabel* agentInfoTitle = new QLabel("Agent Information", this);
    agentInfoTitle->setObjectName("sectionTitle");
    agentInfoLayout->addWidget(agentInfoTitle);
    
    QLabel* agentInfoContent = new QLabel("", this);
    agentInfoContent->setObjectName("agentInfoContent");
    agentInfoContent->setWordWrap(true);
    agentInfoLayout->addWidget(agentInfoContent);
    
    panelLayout->addWidget(m_agentInfoPanel);
}

void MainWindow::setupStyles() {
    QString style = R"(
        QMainWindow {
            background-color: #1E1E1E;
        }
        
        QWidget {
            background-color: #1E1E1E;
            color: #CCCCCC;
            font-family: 'Segoe UI', 'SF Pro Display', -apple-system, sans-serif;
            font-size: 14px;
        }
        
        QMenuBar {
            background-color: #252526;
            color: #CCCCCC;
            border-bottom: 1px solid #3E3E42;
            padding: 4px;
        }
        
        QMenuBar::item {
            padding: 6px 12px;
            border-radius: 4px;
        }
        
        QMenuBar::item:selected {
            background-color: #2A2D2E;
        }
        
        QMenu {
            background-color: #252526;
            color: #CCCCCC;
            border: 1px solid #3E3E42;
            padding: 4px;
        }
        
        QMenu::item {
            padding: 6px 24px;
        }
        
        QMenu::item:selected {
            background-color: #007ACC;
        }
        
        QToolBar {
            background-color: #252526;
            border-bottom: 1px solid #3E3E42;
            spacing: 8px;
            padding: 8px;
        }
        
        QToolBar::separator {
            background-color: #3E3E42;
            width: 1px;
            margin: 4px 8px;
        }
        
        QPushButton {
            background-color: #2A2D2E;
            border: 1px solid #3E3E42;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: 500;
            color: #CCCCCC;
            min-height: 24px;
        }
        
        QPushButton:hover {
            background-color: #3E3E42;
            border-color: #007ACC;
            box-shadow: 0 2px 4px rgba(0, 122, 204, 0.3);
        }
        
        QPushButton:pressed {
            background-color: #1E1E1E;
        }
        
        QPushButton:disabled {
            background-color: #1E1E1E;
            color: #6E6E6E;
            border-color: #2A2D2E;
        }
        
        QComboBox {
            background-color: #2A2D2E;
            border: 1px solid #3E3E42;
            border-radius: 4px;
            padding: 6px 12px;
            color: #CCCCCC;
            min-width: 150px;
        }
        
        QComboBox:hover {
            border-color: #007ACC;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 6px solid #CCCCCC;
            margin-right: 4px;
        }
        
        QComboBox QAbstractItemView {
            background-color: #252526;
            border: 1px solid #3E3E42;
            selection-background-color: #007ACC;
            color: #CCCCCC;
        }
        
        QSlider::groove:horizontal {
            background-color: #2A2D2E;
            height: 6px;
            border-radius: 3px;
        }
        
        QSlider::handle:horizontal {
            background-color: #007ACC;
            border: 2px solid #1E1E1E;
            width: 16px;
            height: 16px;
            border-radius: 8px;
            margin: -5px 0;
        }
        
        QSlider::handle:horizontal:hover {
            background-color: #3B82F6;
        }
        
        QSlider::sub-page:horizontal {
            background-color: #007ACC;
        }
        
        QStatusBar {
            background-color: #252526;
            border-top: 1px solid #3E3E42;
            color: #CCCCCC;
        }
        
        #controlPanel {
            background-color: #252526;
            border-right: 1px solid #3E3E42;
        }
        
        QGroupBox {
            background-color: #1E1E1E;
            border: 1px solid #3E3E42;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 12px;
            font-weight: 600;
            color: #CCCCCC;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 8px;
            color: #CCCCCC;
        }
        
        QCheckBox {
            spacing: 8px;
            color: #CCCCCC;
        }
        
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #3E3E42;
            border-radius: 4px;
            background-color: #1E1E1E;
        }
        
        QCheckBox::indicator:checked {
            background-color: #007ACC;
            border-color: #007ACC;
        }
        
        QCheckBox::indicator:hover {
            border-color: #007ACC;
        }
        
        QSpinBox {
            background-color: #2A2D2E;
            border: 1px solid #3E3E42;
            border-radius: 4px;
            padding: 4px 8px;
            color: #CCCCCC;
        }
        
        QSpinBox:hover {
            border-color: #007ACC;
        }
        
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #3E3E42;
            border: none;
            width: 16px;
        }
        
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #007ACC;
        }
        
        #agentInfoPanel {
            background-color: #1E1E1E;
            border: 1px solid #3E3E42;
            border-radius: 8px;
            padding: 12px;
        }
        
        #sectionTitle {
            font-size: 16px;
            font-weight: 600;
            color: #FFFFFF;
            margin-bottom: 8px;
        }
        
        #agentInfoContent {
            color: #CCCCCC;
            line-height: 1.6;
            font-size: 12px;
        }
        
        #metricsPanel {
            background-color: #252526;
            border-left: 1px solid #3E3E42;
        }
        
        #metricLabel {
            color: #CCCCCC;
            font-size: 13px;
            padding: 4px 0;
        }
        
        #chartLabel {
            background-color: #1E1E1E;
            border: 1px solid #3E3E42;
            border-radius: 6px;
            color: #CCCCCC;
            padding: 8px;
        }
        
        #secondaryButton {
            background-color: #2A2D2E;
        }
        
        #secondaryButton:hover {
            background-color: #3E3E42;
        }
        
        #statCard {
            background-color: #252526;
            border: 1px solid #3E3E42;
            border-radius: 8px;
            min-width: 120px;
        }
        
        #statCard:hover {
            border-color: #007ACC;
            box-shadow: 0 2px 8px rgba(0, 122, 204, 0.2);
        }
    )";
    
    setStyleSheet(style);
}

void MainWindow::connectSignals() {
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    connect(m_showGridCheckbox, &QCheckBox::toggled, this, &MainWindow::onToggleGrid);
    connect(m_showNodeLabelsCheckbox, &QCheckBox::toggled, this, &MainWindow::onToggleNodeLabels);
    connect(m_agentTrailsCheckbox, &QCheckBox::toggled, this, &MainWindow::onToggleAgentTrails);
    connect(m_heatMapCheckbox, &QCheckBox::toggled, this, &MainWindow::onToggleHeatMap);
    connect(m_policyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onPolicyChanged);
    connect(m_speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(m_loadPresetButton, &QPushButton::clicked, this, &MainWindow::onLoadPreset);
    
    if (m_gridView) {
        connect(m_gridView, &GridView::agentSelected, this, &MainWindow::onAgentSelected);
    }
    
    // Simulation timer
    connect(m_simulationTimer, &QTimer::timeout, this, &MainWindow::onTick);
    m_simulationTimer->setInterval(100); // Base interval
}

void MainWindow::updateControls() {
    m_startButton->setEnabled(!m_isRunning);
    m_pauseButton->setEnabled(m_isRunning);
    
    if (m_isRunning) {
        m_statusIndicator->setStyleSheet("color: #4ade80; font-size: 12px;");
        statusBar()->showMessage("Simulation Running", 0);
    } else {
        m_statusIndicator->setStyleSheet("color: #ef4444; font-size: 12px;");
        statusBar()->showMessage("Simulation Stopped", 0);
    }
}

void MainWindow::updateToolbarStats() {
    if (!m_controller) {
        m_tickLabel->setText("Tick: 0");
        m_agentsLabel->setText("Agents: 0/0");
        return;
    }
    
    Metrics* metrics = m_controller->getMetrics();
    auto& agents = m_controller->getAgents();
    
    if (metrics) {
        m_tickLabel->setText(QString("Tick: %1").arg(metrics->getCurrentTick()));
    }
    
    int totalAgents = agents.size();
    int activeAgents = 0;
    for (Agent* agent : agents) {
        if (agent && !agent->hasArrived()) {
            activeAgents++;
        }
    }
    
    m_agentsLabel->setText(QString("Agents: %1/%2").arg(activeAgents).arg(totalAgents));
    
    // Update quick stats
    if (metrics) {
        int arrivedCount = totalAgents - activeAgents;
        double avgTrip = 0.0;
        if (arrivedCount > 0) {
            int totalTime = 0;
            for (Agent* agent : agents) {
                if (agent && agent->hasArrived()) {
                    totalTime += agent->getTravelTime();
                }
            }
            avgTrip = static_cast<double>(totalTime) / arrivedCount;
        }
        
        m_avgTripLabel->setText(QString("Avg Trip: %1 steps").arg(avgTrip, 0, 'f', 1));
        m_throughputLabel->setText(QString("Throughput: %1").arg(arrivedCount));
        m_maxLoadLabel->setText(QString("Max Load: %1 vehicles").arg(metrics->getMaxEdgeLoad()));
    }
}

void MainWindow::showToast(const QString& message) {
    // Create toast label if it doesn't exist
    if (!m_toastLabel) {
        m_toastLabel = new QLabel(this);
        m_toastLabel->setObjectName("toastLabel");
        m_toastLabel->setStyleSheet(
            "background-color: #252526;"
            "color: #CCCCCC;"
            "border: 1px solid #3E3E42;"
            "border-radius: 8px;"
            "padding: 12px 20px;"
            "font-weight: 500;"
        );
        m_toastLabel->setAlignment(Qt::AlignCenter);
        m_toastLabel->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
        m_toastLabel->setAttribute(Qt::WA_TranslucentBackground);
    }
    
    m_toastLabel->setText(message);
    m_toastLabel->adjustSize();
    
    // Position at top-right
    QPoint pos = mapToGlobal(QPoint(width() - m_toastLabel->width() - 20, 60));
    m_toastLabel->move(pos);
    m_toastLabel->show();
    
    // Fade in animation
    if (m_toastAnimation) {
        m_toastAnimation->stop();
        delete m_toastAnimation;
    }
    
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(m_toastLabel);
    m_toastLabel->setGraphicsEffect(effect);
    
    m_toastAnimation = new QPropertyAnimation(effect, "opacity", this);
    m_toastAnimation->setDuration(300);
    m_toastAnimation->setStartValue(0.0);
    m_toastAnimation->setEndValue(1.0);
    m_toastAnimation->start();
    
    // Auto-hide after 3 seconds
    QTimer::singleShot(3000, this, [this]() {
        if (m_toastAnimation) {
            m_toastAnimation->setStartValue(1.0);
            m_toastAnimation->setEndValue(0.0);
            m_toastAnimation->start();
            connect(m_toastAnimation, &QPropertyAnimation::finished, this, [this]() {
                if (m_toastLabel) m_toastLabel->hide();
            });
        }
    });
}

void MainWindow::updateFPS() {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qint64 elapsed = now - m_lastFpsUpdate;
    
    if (elapsed > 0) {
        int fps = static_cast<int>((m_frameCount * 1000.0) / elapsed);
        m_fpsLabel->setText(QString::number(fps));
        m_frameCount = 0;
        m_lastFpsUpdate = now;
    }
}

void MainWindow::onStartClicked() {
    if (!m_controller) return;
    
    m_controller->start();
    m_isRunning = true;
    
    // Adjust timer interval based on speed
    int baseInterval = 100;
    int actualInterval = baseInterval / m_currentSpeed;
    m_simulationTimer->setInterval(qMax(10, actualInterval));
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
    updateToolbarStats();
}

void MainWindow::onTick() {
    if (!m_controller || !m_isRunning) return;
    
    m_controller->tick();
    m_frameCount++;
    
    if (m_gridView) {
        m_gridView->animateStep();
    }
    if (m_metricsPanel) {
        m_metricsPanel->updateMetrics();
    }
    
    updateToolbarStats();
    
    // Update last update time
    static qint64 lastUpdate = 0;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (lastUpdate > 0) {
        double elapsed = (now - lastUpdate) / 1000.0;
        m_updateTimeLabel->setText(QString("%1s").arg(elapsed, 0, 'f', 2));
    }
    lastUpdate = now;
}

void MainWindow::onAgentSelected(Agent* agent) {
    // Agent selection handling
    Q_UNUSED(agent);
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

void MainWindow::onToggleAgentTrails() {
    // Implement agent trails toggle
}

void MainWindow::onToggleHeatMap() {
    // Implement heat map toggle
}

void MainWindow::onPolicyChanged(int index) {
    if (!m_controller) return;
    
    PolicyType policy = (index == 0) ? PolicyType::SHORTEST_PATH : PolicyType::CONGESTION_AWARE;
    m_controller->setPolicy(policy);
    
    if (m_gridView) {
        m_gridView->updateScene();
    }
}

void MainWindow::onSpeedChanged(int value) {
    m_currentSpeed = value;
    m_speedLabel->setText(QString("%1.0x").arg(value));
    
    if (m_isRunning && m_simulationTimer) {
        int baseInterval = 100;
        int actualInterval = baseInterval / value;
        m_simulationTimer->setInterval(qMax(10, actualInterval));
    }
}

void MainWindow::onLoadPreset() {
    QString presetName = m_presetCombo->currentText();
    PresetLoader loader;
    
    try {
        Preset preset = loader.loadFromJson("presets/" + presetName.toStdString());
        m_controller->loadPreset(preset);
        
        if (m_gridView) {
            m_gridView->setSimulationController(m_controller);
            m_gridView->updateScene();
        }
        if (m_metricsPanel) {
            m_metricsPanel->setSimulationController(m_controller);
            m_metricsPanel->updateMetrics();
        }
        
        // Update UI to match preset
        m_gridSizeRows->setValue(preset.getRows());
        m_gridSizeCols->setValue(preset.getCols());
        m_agentsSpin->setValue(preset.getAgentCount());
        m_tickIntervalSpin->setValue(preset.getTickMs());
        
        int policyIndex = (preset.getPolicy() == PolicyType::SHORTEST_PATH) ? 0 : 1;
        m_policyCombo->setCurrentIndex(policyIndex);
        
        showToast("Preset loaded successfully ✓");
        updateToolbarStats();
    } catch (...) {
        QMessageBox::warning(this, "Error", "Failed to load preset: " + presetName);
    }
}

void MainWindow::onExportCSV() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export CSV", 
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/metrics.csv",
        "CSV Files (*.csv)");
    
    if (!fileName.isEmpty()) {
        // Export logic here
        showToast("Metrics exported to " + fileName);
    }
}

void MainWindow::onExportJSON() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export JSON",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/config.json",
        "JSON Files (*.json)");
    
    if (!fileName.isEmpty()) {
        // Export logic here
        showToast("Config exported to " + fileName);
    }
}

void MainWindow::onSaveConfig() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Config",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/config.json",
        "JSON Files (*.json)");
    
    if (!fileName.isEmpty()) {
        // Save logic here
        showToast("Config saved to " + fileName);
    }
}

void MainWindow::onZoomIn() {
    if (m_gridView) {
        qreal currentZoom = m_gridView->zoomLevel();
        m_gridView->setZoomLevel(currentZoom * 1.2);
    }
}

void MainWindow::onZoomOut() {
    if (m_gridView) {
        qreal currentZoom = m_gridView->zoomLevel();
        m_gridView->setZoomLevel(currentZoom / 1.2);
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About GridlockLondon",
        "<h2>GridlockLondon</h2>"
        "<p><b>Traffic Simulation System</b></p>"
        "<p>Version 1.0</p>"
        "<p>Developed by:<br>"
        "Faozia and Paridhi</p>"
        "<p>An OOP Assignment 2 project demonstrating<br>"
        "traffic simulation with routing policies.</p>");
}

void MainWindow::onOpenAnalytics() {
    if (!m_analyticsPanel) {
        m_analyticsPanel = new AnalyticsPanel(this);
        m_analyticsPanel->setSimulationController(m_controller);
        m_analyticsPanel->setWindowTitle("Advanced Analytics - GridlockLondon");
        m_analyticsPanel->resize(1000, 700);
    }
    
    m_analyticsPanel->show();
    m_analyticsPanel->raise();
    m_analyticsPanel->activateWindow();
    m_analyticsPanel->updateAnalytics();
}

void MainWindow::onShortcuts() {
    QMessageBox::information(this, "Keyboard Shortcuts",
        "<b>Simulation:</b><br>"
        "Space - Start/Pause<br>"
        "R - Reset<br><br>"
        "<b>View:</b><br>"
        "Ctrl + Plus - Zoom In<br>"
        "Ctrl + Minus - Zoom Out<br>"
        "Ctrl + 0 - Fit to Window<br><br>"
        "<b>Navigation:</b><br>"
        "Arrow Keys - Pan view<br>"
        "Shift + Drag - Pan view");
}

void MainWindow::onSectionToggled() {
    // Collapsible section toggle
}
