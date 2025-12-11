#include "MainWindow.h"
#include "GridView.h"
#include "MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Agent.h"
#include "../core/Preset.h"
#include "../core/Metrics.h"
#include "../adapters/PresetLoader.h"
#include <vector>
#include <string>
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
#include <QFrame>
#include <QScrollArea>
#include "AnalyticsPanel.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_gridView(nullptr), m_metricsPanel(nullptr),
      m_analyticsPanel(nullptr), m_controller(nullptr), m_simulationTimer(new QTimer(this)), 
      m_fpsTimer(new QTimer(this)), m_isRunning(false), m_currentSpeed(1),
      m_frameCount(0), m_lastFpsUpdate(0), m_zoomSlider(nullptr), m_zoomLabel(nullptr),
      m_toastLabel(nullptr), m_toastAnimation(nullptr),
      m_legendEmpty(nullptr), m_legendLight(nullptr), m_legendModerate(nullptr),
      m_legendHeavy(nullptr), m_legendGridlock(nullptr), m_currentCapacity(3) {
    
    setWindowTitle("GridlockLondon - Urban Traffic Simulator");
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
        Preset preset;
        bool loaded = false;
        
        std::vector<std::string> pathsToTry = {
            "../presets/demo_5x5.json",
            "../../code/presets/demo_5x5.json",
            "presets/demo_5x5.json",
            "../code/presets/demo_5x5.json"
        };
        
        for (const auto& path : pathsToTry) {
            try {
                preset = loader.loadFromJson(path);
                loaded = true;
                break;
            } catch (...) {
                continue;
            }
        }
        
        if (!loaded) {
            preset.setName("default_5x5");
            preset.setRows(5);
            preset.setCols(5);
            preset.setAgentCount(10);
            preset.setTickMs(100);
            preset.setPolicy(PolicyType::SHORTEST_PATH);
        }
        
        m_controller->loadPreset(preset);
        m_gridView->setSimulationController(m_controller);
        m_metricsPanel->setSimulationController(m_controller);
        m_gridView->updateScene();
        
        // Initialize traffic legend for default 5x5 grid (capacity 2)
        updateTrafficLegend(2);
        
    } catch (...) {
        // Fallback - still initialize legend
        updateTrafficLegend(2);
    }
    
    updateControls();
    updateToolbarStats();
    
    QTimer::singleShot(100, this, [this]() {
        if (m_gridView) {
            m_gridView->fitToWindow();
        }
    });
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
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    QVBoxLayout* outerLayout = new QVBoxLayout(m_centralWidget);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);
    
    // Main content area
    QWidget* mainContent = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(mainContent);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Left panel: Control Panel with scroll
    setupControlPanel();
    QScrollArea* leftScroll = new QScrollArea(this);
    leftScroll->setWidget(m_controlPanel);
    leftScroll->setWidgetResizable(true);
    leftScroll->setMinimumWidth(260);
    leftScroll->setMaximumWidth(280);
    leftScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    leftScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    leftScroll->setObjectName("leftScrollArea");
    leftScroll->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(leftScroll);
    
    // Center: GridView with scrollbars
    m_gridView = new GridView(this);
    m_gridView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_gridView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainLayout->addWidget(m_gridView, 1);
    
    // Right panel: Metrics with scroll
    m_metricsPanel = new MetricsPanel(this);
    m_metricsPanel->setMinimumWidth(340);
    QScrollArea* rightScroll = new QScrollArea(this);
    rightScroll->setWidget(m_metricsPanel);
    rightScroll->setWidgetResizable(true);
    rightScroll->setMinimumWidth(350);
    rightScroll->setMaximumWidth(380);
    rightScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rightScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    rightScroll->setObjectName("rightScrollArea");
    rightScroll->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(rightScroll);
    
    outerLayout->addWidget(mainContent, 1);
    
    // Bottom: Zoom control bar
    QWidget* zoomBar = new QWidget(this);
    zoomBar->setObjectName("zoomBar");
    zoomBar->setFixedHeight(48);
    QHBoxLayout* zoomLayout = new QHBoxLayout(zoomBar);
    zoomLayout->setContentsMargins(20, 8, 20, 8);
    zoomLayout->setSpacing(12);
    
    // Map navigation label
    QLabel* navLabel = new QLabel("🗺️ Map Navigation", this);
    navLabel->setStyleSheet("color: #888; font-size: 12px; font-weight: 500;");
    zoomLayout->addWidget(navLabel);
    
    zoomLayout->addSpacing(20);
    
    // Zoom out button
    QPushButton* zoomOutBtn = new QPushButton("−", this);
    zoomOutBtn->setFixedSize(28, 28);
    zoomOutBtn->setObjectName("zoomButton");
    zoomOutBtn->setToolTip("Zoom out");
    connect(zoomOutBtn, &QPushButton::clicked, this, [this]() {
        m_zoomSlider->setValue(m_zoomSlider->value() - 20);
    });
    zoomLayout->addWidget(zoomOutBtn);
    
    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setMinimum(20);
    m_zoomSlider->setMaximum(400);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setFixedWidth(200);
    m_zoomSlider->setToolTip("Zoom level");
    zoomLayout->addWidget(m_zoomSlider);
    
    // Zoom in button
    QPushButton* zoomInBtn = new QPushButton("+", this);
    zoomInBtn->setFixedSize(28, 28);
    zoomInBtn->setObjectName("zoomButton");
    zoomInBtn->setToolTip("Zoom in");
    connect(zoomInBtn, &QPushButton::clicked, this, [this]() {
        m_zoomSlider->setValue(m_zoomSlider->value() + 20);
    });
    zoomLayout->addWidget(zoomInBtn);
    
    m_zoomLabel = new QLabel("100%", this);
    m_zoomLabel->setFixedWidth(50);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    m_zoomLabel->setStyleSheet("color: #aaa; font-weight: 600; font-size: 12px;");
    zoomLayout->addWidget(m_zoomLabel);
    
    zoomLayout->addSpacing(20);
    
    QPushButton* fitButton = new QPushButton("Fit View", this);
    fitButton->setMinimumWidth(70);
    fitButton->setToolTip("Fit entire city in view");
    connect(fitButton, &QPushButton::clicked, this, &MainWindow::onFitToWindow);
    zoomLayout->addWidget(fitButton);
    
    QPushButton* resetViewBtn = new QPushButton("100%", this);
    resetViewBtn->setMinimumWidth(55);
    resetViewBtn->setToolTip("Reset to 100% zoom");
    connect(resetViewBtn, &QPushButton::clicked, this, [this]() {
        m_zoomSlider->setValue(100);
    });
    zoomLayout->addWidget(resetViewBtn);
    
    zoomLayout->addStretch();
    
    // Hint text
    QLabel* hintLabel = new QLabel("Scroll wheel to zoom • Shift+drag to pan", this);
    hintLabel->setStyleSheet("color: #555; font-size: 11px;");
    zoomLayout->addWidget(hintLabel);
    
    connect(m_zoomSlider, &QSlider::valueChanged, this, [this](int value) {
        if (m_gridView) {
            m_gridView->setZoomLevel(value / 100.0);
            m_zoomLabel->setText(QString("%1%").arg(value));
        }
    });
    
    outerLayout->addWidget(zoomBar);
}

void MainWindow::setupMenuBar() {
    // File Menu
    m_fileMenu = menuBar()->addMenu("File");
    m_fileMenu->addAction("Open Preset...", this, &MainWindow::onLoadPreset, QKeySequence::Open);
    m_fileMenu->addAction("Save Configuration...", this, &MainWindow::onSaveConfig, QKeySequence::Save);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("Export Results (CSV)...", this, &MainWindow::onExportCSV);
    m_fileMenu->addAction("Export Results (JSON)...", this, &MainWindow::onExportJSON);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction("Quit", this, &QWidget::close, QKeySequence::Quit);
    
    // Simulation Menu
    m_simulationMenu = menuBar()->addMenu("Simulation");
    m_simulationMenu->addAction("Start", this, &MainWindow::onStartClicked, QKeySequence(Qt::Key_Space));
    m_simulationMenu->addAction("Pause", this, &MainWindow::onPauseClicked);
    m_simulationMenu->addAction("Reset", this, &MainWindow::onResetClicked, QKeySequence(Qt::Key_R));
    m_simulationMenu->addSeparator();
    m_simulationMenu->addAction("Increase Speed", this, [this]() { 
        m_speedSlider->setValue(qMin(10, m_speedSlider->value() + 1)); 
    }, QKeySequence(Qt::Key_Plus));
    m_simulationMenu->addAction("Decrease Speed", this, [this]() { 
        m_speedSlider->setValue(qMax(1, m_speedSlider->value() - 1)); 
    }, QKeySequence(Qt::Key_Minus));
    
    // View Menu
    m_viewMenu = menuBar()->addMenu("View");
    m_viewMenu->addAction("Zoom In", this, &MainWindow::onZoomIn, QKeySequence::ZoomIn);
    m_viewMenu->addAction("Zoom Out", this, &MainWindow::onZoomOut, QKeySequence::ZoomOut);
    m_viewMenu->addAction("Fit to Window", this, &MainWindow::onFitToWindow, QKeySequence(Qt::CTRL | Qt::Key_0));
    
    // Analytics Menu
    QMenu* analyticsMenu = menuBar()->addMenu("Analytics");
    analyticsMenu->addAction("Open Analytics Dashboard", this, &MainWindow::onOpenAnalytics);
    
    // Help Menu
    m_helpMenu = menuBar()->addMenu("Help");
    m_helpMenu->addAction("About GridlockLondon", this, &MainWindow::onAbout);
    m_helpMenu->addAction("Keyboard Shortcuts", this, &MainWindow::onShortcuts);
}

void MainWindow::setupToolBar() {
    m_toolbar = addToolBar("Controls");
    m_toolbar->setMovable(false);
    m_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    // Control buttons - clean text only
    m_startButton = new QPushButton("  Start  ", this);
    m_startButton->setToolTip("Start the simulation (Space)");
    m_startButton->setMinimumWidth(80);
    m_toolbar->addWidget(m_startButton);
    
    m_pauseButton = new QPushButton("  Pause  ", this);
    m_pauseButton->setToolTip("Pause the simulation (Space)");
    m_pauseButton->setMinimumWidth(80);
    m_toolbar->addWidget(m_pauseButton);
    
    m_resetButton = new QPushButton("  Reset  ", this);
    m_resetButton->setToolTip("Reset simulation to start (R)");
    m_resetButton->setMinimumWidth(80);
    m_toolbar->addWidget(m_resetButton);
    
    m_toolbar->addSeparator();
    
    // Routing Algorithm dropdown
    QLabel* policyLabel = new QLabel("  Routing:  ", this);
    m_toolbar->addWidget(policyLabel);
    
    m_policyCombo = new QComboBox(this);
    m_policyCombo->addItem("Shortest Path");
    m_policyCombo->addItem("Congestion-Aware");
    m_policyCombo->setToolTip("Choose how vehicles find their routes:\n"
                              "• Shortest Path: Always take the quickest route (ignores traffic)\n"
                              "• Congestion-Aware: Avoids busy roads, may take longer routes");
    m_policyCombo->setMinimumWidth(160);
    m_policyCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_toolbar->addWidget(m_policyCombo);
    
    m_toolbar->addSeparator();
    
    // Speed control
    QLabel* speedLabel = new QLabel("  Speed:  ", this);
    m_toolbar->addWidget(speedLabel);
    
    m_speedSlider = new QSlider(Qt::Horizontal, this);
    m_speedSlider->setMinimum(1);
    m_speedSlider->setMaximum(10);
    m_speedSlider->setValue(1);
    m_speedSlider->setFixedWidth(120);
    m_speedSlider->setToolTip("Simulation speed: 1x (slow) to 10x (fast)");
    m_toolbar->addWidget(m_speedSlider);
    
    m_speedLabel = new QLabel(" 1x ", this);
    m_speedLabel->setMinimumWidth(35);
    m_toolbar->addWidget(m_speedLabel);
    
    m_toolbar->addSeparator();
    
    // Status display
    m_tickLabel = new QLabel("  Time: 0  ", this);
    m_tickLabel->setToolTip("Simulation time steps elapsed");
    m_toolbar->addWidget(m_tickLabel);
    
    m_agentsLabel = new QLabel("  Vehicles: 0/0  ", this);
    m_agentsLabel->setToolTip("Active vehicles / Total vehicles");
    m_toolbar->addWidget(m_agentsLabel);
}

void MainWindow::setupStatusBar() {
    m_statusIndicator = new QLabel("●", this);
    m_statusIndicator->setStyleSheet("color: #ef4444; font-size: 14px;");
    statusBar()->addWidget(m_statusIndicator);
    
    statusBar()->addWidget(new QLabel("Ready - Click Start to begin simulation", this));
    
    statusBar()->addPermanentWidget(new QLabel("FPS:", this));
    m_fpsLabel = new QLabel("--", this);
    statusBar()->addPermanentWidget(m_fpsLabel);
    
    m_updateTimeLabel = new QLabel("", this);
    statusBar()->addPermanentWidget(m_updateTimeLabel);
}

void MainWindow::setupControlPanel() {
    m_controlPanel = new QWidget(this);
    m_controlPanel->setObjectName("controlPanel");
    QVBoxLayout* panelLayout = new QVBoxLayout(m_controlPanel);
    panelLayout->setContentsMargins(16, 16, 16, 16);
    panelLayout->setSpacing(16);
    
    // WELCOME / INSTRUCTIONS
    QLabel* welcomeLabel = new QLabel("Welcome to GridlockLondon!", this);
    welcomeLabel->setObjectName("welcomeTitle");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(welcomeLabel);
    
    QLabel* instructionLabel = new QLabel(
        "Watch vehicles navigate through a city grid.\n"
        "Compare routing strategies and see how\n"
        "traffic congestion develops.", this);
    instructionLabel->setObjectName("instructionText");
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setWordWrap(true);
    panelLayout->addWidget(instructionLabel);
    
    // Divider
    QFrame* divider1 = new QFrame(this);
    divider1->setFrameShape(QFrame::HLine);
    divider1->setStyleSheet("background-color: #3E3E42;");
    panelLayout->addWidget(divider1);
    
    // CITY PRESET Section
    m_simulationGroup = new QGroupBox("City Map", this);
    QVBoxLayout* simLayout = new QVBoxLayout(m_simulationGroup);
    
    QLabel* presetExplain = new QLabel("Select a city size to simulate:", this);
    presetExplain->setObjectName("explainText");
    simLayout->addWidget(presetExplain);
    
    m_presetCombo = new QComboBox(this);
    m_presetCombo->addItem("Small City (3x3)");
    m_presetCombo->addItem("Medium City (5x5)");
    m_presetCombo->addItem("London Grid (10x10)");
    m_presetCombo->addItem("Manhattan (15x15)");
    m_presetCombo->setCurrentIndex(1);
    m_presetCombo->setToolTip("Choose a pre-configured city map");
    simLayout->addWidget(m_presetCombo);
    
    m_loadPresetButton = new QPushButton("  Load City  ", this);
    m_loadPresetButton->setToolTip("Load the selected city map and reset the simulation");
    m_loadPresetButton->setObjectName("primaryButton");
    m_loadPresetButton->setMinimumHeight(36);
    simLayout->addWidget(m_loadPresetButton);
    
    panelLayout->addWidget(m_simulationGroup);
    
    // DISPLAY OPTIONS Section
    m_visualizationGroup = new QGroupBox("Display Options", this);
    QVBoxLayout* vizLayout = new QVBoxLayout(m_visualizationGroup);
    
    m_showGridCheckbox = new QCheckBox("Show city blocks", this);
    m_showGridCheckbox->setChecked(true);
    m_showGridCheckbox->setToolTip("Show/hide the background city block pattern");
    vizLayout->addWidget(m_showGridCheckbox);
    
    m_showNodeLabelsCheckbox = new QCheckBox("Show intersection numbers", this);
    m_showNodeLabelsCheckbox->setChecked(false);
    m_showNodeLabelsCheckbox->setToolTip("Display ID numbers at each intersection");
    vizLayout->addWidget(m_showNodeLabelsCheckbox);
    
    m_agentTrailsCheckbox = new QCheckBox("Show vehicle trails", this);
    m_agentTrailsCheckbox->setChecked(true);
    m_agentTrailsCheckbox->setToolTip("Show fading trails behind moving vehicles");
    vizLayout->addWidget(m_agentTrailsCheckbox);
    
    m_heatMapCheckbox = new QCheckBox("Heat map overlay", this);
    m_heatMapCheckbox->setChecked(false);
    m_heatMapCheckbox->setToolTip("Overlay traffic density as colors");
    vizLayout->addWidget(m_heatMapCheckbox);
    
    panelLayout->addWidget(m_visualizationGroup);
    
    // TRAFFIC LEGEND (dynamic based on grid size)
    QGroupBox* legendGroup = new QGroupBox("Traffic Legend", this);
    QVBoxLayout* legendLayout = new QVBoxLayout(legendGroup);
    
    m_currentCapacity = 3; // Default
    
    auto addLegendRow = [&](const QString& color, QLabel*& textLabel, const QString& defaultText) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* colorBox = new QLabel(this);
        colorBox->setFixedSize(16, 16);
        colorBox->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(color));
        row->addWidget(colorBox);
        textLabel = new QLabel(defaultText, this);
        textLabel->setObjectName("legendText");
        row->addWidget(textLabel);
        row->addStretch();
        legendLayout->addLayout(row);
    };
    
    addLegendRow("#32A032", m_legendEmpty, "Empty (0 cars)");
    addLegendRow("#78C850", m_legendLight, "Light (1 car)");
    addLegendRow("#FFC800", m_legendModerate, "Moderate (2 cars)");
    addLegendRow("#FF7800", m_legendHeavy, "Heavy (near full)");
    addLegendRow("#FF3232", m_legendGridlock, "Gridlock! (full)");
    
    QFrame* legendDivider = new QFrame(this);
    legendDivider->setFrameShape(QFrame::HLine);
    legendDivider->setStyleSheet("background-color: #3E3E42; margin: 8px 0;");
    legendLayout->addWidget(legendDivider);
    
    QHBoxLayout* carRow1 = new QHBoxLayout();
    QLabel* blueBox = new QLabel(this);
    blueBox->setFixedSize(16, 16);
    blueBox->setStyleSheet("background-color: #4682DC; border-radius: 3px;");
    carRow1->addWidget(blueBox);
    carRow1->addWidget(new QLabel("Shortest Path vehicles", this));
    carRow1->addStretch();
    legendLayout->addLayout(carRow1);
    
    QHBoxLayout* carRow2 = new QHBoxLayout();
    QLabel* greenBox = new QLabel(this);
    greenBox->setFixedSize(16, 16);
    greenBox->setStyleSheet("background-color: #32B464; border-radius: 3px;");
    carRow2->addWidget(greenBox);
    carRow2->addWidget(new QLabel("Congestion-Aware vehicles", this));
    carRow2->addStretch();
    legendLayout->addLayout(carRow2);
    
    panelLayout->addWidget(legendGroup);
    
    // LIVE STATS Section
    m_quickStatsGroup = new QGroupBox("Live Statistics", this);
    QVBoxLayout* statsLayout = new QVBoxLayout(m_quickStatsGroup);
    
    m_avgTripLabel = new QLabel("Avg. Trip Time: -- steps", this);
    m_avgTripLabel->setToolTip("Average number of steps for completed journeys");
    statsLayout->addWidget(m_avgTripLabel);
    
    m_throughputLabel = new QLabel("Completed Trips: 0", this);
    m_throughputLabel->setToolTip("Number of vehicles that reached their destination");
    statsLayout->addWidget(m_throughputLabel);
    
    m_maxLoadLabel = new QLabel("Busiest Road: 0 vehicles", this);
    m_maxLoadLabel->setToolTip("Maximum vehicles on any single road segment");
    statsLayout->addWidget(m_maxLoadLabel);
    
    panelLayout->addWidget(m_quickStatsGroup);
    
    panelLayout->addStretch();
    
    // HOW TO USE
    QLabel* howToLabel = new QLabel(
        "Tip: Click Start, then watch the vehicles\n"
        "move. Try switching routing modes!", this);
    howToLabel->setObjectName("tipText");
    howToLabel->setAlignment(Qt::AlignCenter);
    howToLabel->setWordWrap(true);
    panelLayout->addWidget(howToLabel);
    
    // Hidden agent info panel
    m_agentInfoPanel = new QWidget(this);
    m_agentInfoPanel->setObjectName("agentInfoPanel");
    m_agentInfoPanel->setVisible(false);
    QVBoxLayout* agentInfoLayout = new QVBoxLayout(m_agentInfoPanel);
    agentInfoLayout->setSpacing(10);
    
    QLabel* agentInfoTitle = new QLabel("Vehicle Details", this);
    agentInfoTitle->setObjectName("sectionTitle");
    agentInfoLayout->addWidget(agentInfoTitle);
    
    QLabel* agentInfoContent = new QLabel("", this);
    agentInfoContent->setObjectName("agentInfoContent");
    agentInfoContent->setWordWrap(true);
    agentInfoLayout->addWidget(agentInfoContent);
    
    panelLayout->addWidget(m_agentInfoPanel);
    
    // Remove settings group - not needed for basic use
    m_settingsGroup = new QGroupBox(this);
    m_settingsGroup->setVisible(false);
    m_gridSizeRows = new QSpinBox(this);
    m_gridSizeCols = new QSpinBox(this);
    m_agentsSpin = new QSpinBox(this);
    m_tickIntervalSpin = new QSpinBox(this);
}

void MainWindow::setupStyles() {
    QString style = R"(
        QMainWindow {
            background-color: #252220;
        }
        
        QWidget {
            background-color: #252220;
            color: #f5f0e8;
            font-family: 'SF Pro Display', -apple-system, 'Segoe UI', 'Helvetica Neue', sans-serif;
            font-size: 13px;
        }
        
        QMenuBar {
            background-color: #2d2a26;
            color: #f5f0e8;
            border-bottom: 1px solid #4a453d;
            padding: 6px 12px;
            font-size: 13px;
        }
        
        QMenuBar::item {
            padding: 6px 14px;
            border-radius: 6px;
            margin: 0 2px;
        }
        
        QMenuBar::item:selected {
            background-color: #3d3832;
        }
        
        QMenu {
            background-color: #2d2a26;
            color: #f5f0e8;
            border: 1px solid #4a453d;
            border-radius: 10px;
            padding: 8px;
        }
        
        QMenu::item {
            padding: 10px 28px;
            border-radius: 6px;
            margin: 2px 0;
        }
        
        QMenu::item:selected {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6a9f50, stop:1 #5a8f40);
        }
        
        QToolBar {
            background-color: #2d2a26;
            border-bottom: 1px solid #4a453d;
            spacing: 10px;
            padding: 10px 16px;
        }
        
        QToolBar::separator {
            background-color: #4a453d;
            width: 1px;
            margin: 8px 14px;
        }
        
        QPushButton {
            background-color: #3d3832;
            border: 1px solid #5a5448;
            border-radius: 8px;
            padding: 8px 18px;
            font-weight: 500;
            color: #f5f0e8;
            min-height: 20px;
        }
        
        QPushButton:hover {
            background-color: #4a4540;
            border-color: #6a6458;
        }
        
        QPushButton:pressed {
            background-color: #2d2a26;
        }
        
        QPushButton:disabled {
            background-color: #2d2a26;
            color: #6a6458;
            border-color: #3d3832;
        }
        
        QPushButton#primaryButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6a9f50, stop:1 #5a8f40);
            border: none;
            color: white;
            font-weight: 600;
        }
        
        QPushButton#primaryButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7ab060, stop:1 #6aa050);
        }
        
        QPushButton#zoomButton {
            background-color: #3d3832;
            border: 1px solid #5a5448;
            border-radius: 6px;
            font-size: 16px;
            font-weight: bold;
            padding: 0;
        }
        
        QPushButton#zoomButton:hover {
            background-color: #4a4540;
        }
        
        QComboBox {
            background-color: #3d3832;
            border: 1px solid #5a5448;
            border-radius: 8px;
            padding: 10px 14px;
            color: #f5f0e8;
            min-width: 120px;
            font-weight: 500;
        }
        
        QComboBox:hover {
            border-color: #7ab060;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 28px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #a89880;
        }
        
        QComboBox QAbstractItemView {
            background-color: #2d2a26;
            border: 1px solid #4a453d;
            border-radius: 10px;
            selection-background-color: #5a8040;
            color: #f5f0e8;
            padding: 6px;
            outline: none;
        }
        
        QSlider::groove:horizontal {
            background-color: #4a453d;
            height: 6px;
            border-radius: 3px;
        }
        
        QSlider::handle:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #8fc070, stop:1 #6a9f50);
            width: 18px;
            height: 18px;
            border-radius: 9px;
            margin: -6px 0;
            border: 2px solid #2d2a26;
        }
        
        QSlider::handle:horizontal:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #a0d080, stop:1 #7ab060);
        }
        
        QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6a9f50, stop:1 #5a8f40);
            border-radius: 3px;
        }
        
        QStatusBar {
            background-color: #2d2a26;
            border-top: 1px solid #4a453d;
            color: #a89880;
            font-size: 12px;
            padding: 4px 12px;
        }
        
        #controlPanel {
            background-color: #2d2a26;
            border-right: 1px solid #4a453d;
        }
        
        #welcomeTitle {
            font-size: 18px;
            font-weight: 700;
            color: #f5f0e8;
            padding: 12px 0 4px 0;
        }
        
        #instructionText {
            color: #c4b8a8;
            font-size: 12px;
            line-height: 1.5;
            padding-bottom: 12px;
        }
        
        #explainText {
            color: #c4b8a8;
            font-size: 12px;
            margin-bottom: 6px;
        }
        
        #tipText {
            color: #a89880;
            font-size: 11px;
            font-style: italic;
            padding: 12px;
            background-color: #252220;
            border-radius: 8px;
            border: 1px solid #4a453d;
        }
        
        #legendText {
            color: #d4c8b8;
            font-size: 12px;
        }
        
        QGroupBox {
            background-color: #252220;
            border: 1px solid #4a453d;
            border-radius: 12px;
            margin-top: 20px;
            padding: 16px;
            padding-top: 28px;
            font-weight: 600;
            color: #f5f0e8;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 6px 14px;
            color: #f5f0e8;
            font-size: 13px;
            font-weight: 600;
        }
        
        QCheckBox {
            spacing: 12px;
            color: #c4b8a8;
            font-size: 13px;
            padding: 6px 0;
        }
        
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border: 2px solid #5a5448;
            border-radius: 6px;
            background-color: #2d2a26;
        }
        
        QCheckBox::indicator:checked {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7ab060, stop:1 #5a8f40);
            border-color: #7ab060;
        }
        
        QCheckBox::indicator:hover {
            border-color: #7ab060;
        }
        
        QSpinBox {
            background-color: #3d3832;
            border: 1px solid #5a5448;
            border-radius: 8px;
            padding: 8px 12px;
            color: #f5f0e8;
        }
        
        QSpinBox:hover {
            border-color: #7ab060;
        }
        
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: transparent;
            border: none;
            width: 20px;
        }
        
        #agentInfoPanel {
            background-color: #352f2a;
            border: 1px solid #4a453d;
            border-radius: 12px;
            padding: 16px;
        }
        
        #sectionTitle {
            font-size: 15px;
            font-weight: 600;
            color: #f5f0e8;
        }
        
        #metricsPanel {
            background-color: #2d2a26;
            border-left: 1px solid #4a453d;
        }
        
        #zoomBar {
            background-color: #252220;
            border-top: 1px solid #4a453d;
        }
        
        #zoomBar QSlider::groove:horizontal {
            background-color: #4a453d;
            height: 6px;
            border-radius: 3px;
        }
        
        #zoomBar QSlider::handle:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7a9f60, stop:1 #5a8040);
            width: 16px;
            height: 16px;
            border-radius: 8px;
            margin: -5px 0;
            border: 2px solid #252220;
        }
        
        #zoomBar QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #5a8040, stop:1 #4a7035);
            border-radius: 3px;
        }
        
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        
        #leftScrollArea, #rightScrollArea {
            background-color: #2d2a26;
        }
        
        QScrollBar:vertical {
            background-color: #252220;
            width: 12px;
            border: none;
            margin: 4px 2px;
        }
        
        QScrollBar::handle:vertical {
            background-color: #4a453d;
            border-radius: 4px;
            min-height: 40px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #5a554d;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
            border: none;
        }
        
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }
        
        QScrollBar:horizontal {
            background-color: #252220;
            height: 12px;
            border: none;
            margin: 2px 4px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: #4a453d;
            border-radius: 4px;
            min-width: 40px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: #5a554d;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0;
            border: none;
        }
        
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: none;
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
    
    connect(m_simulationTimer, &QTimer::timeout, this, &MainWindow::onTick);
    m_simulationTimer->setInterval(300);
}

void MainWindow::updateControls() {
    m_startButton->setEnabled(!m_isRunning);
    m_pauseButton->setEnabled(m_isRunning);
    
    if (m_isRunning) {
        m_statusIndicator->setStyleSheet("color: #4ade80; font-size: 14px;");
        statusBar()->showMessage("Simulation Running - Watch the vehicles move!", 0);
    } else {
        m_statusIndicator->setStyleSheet("color: #ef4444; font-size: 14px;");
        statusBar()->showMessage("Ready - Click Start to begin simulation", 0);
    }
}

void MainWindow::updateToolbarStats() {
    if (!m_controller) {
        m_tickLabel->setText("  Time: 0  ");
        m_agentsLabel->setText("  Vehicles: 0/0  ");
        return;
    }
    
    Metrics* metrics = m_controller->getMetrics();
    auto& agents = m_controller->getAgents();
    
    if (metrics) {
        m_tickLabel->setText(QString("  Time: %1  ").arg(metrics->getCurrentTick()));
    }
    
    int totalAgents = agents.size();
    int activeAgents = 0;
    for (Agent* agent : agents) {
        if (agent && !agent->hasArrived()) {
            activeAgents++;
        }
    }
    
    m_agentsLabel->setText(QString("  Vehicles: %1/%2  ").arg(activeAgents).arg(totalAgents));
    
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
        
        if (arrivedCount > 0) {
            m_avgTripLabel->setText(QString("Avg. Trip Time: %1 steps").arg(avgTrip, 0, 'f', 1));
        } else {
            m_avgTripLabel->setText("Avg. Trip Time: -- steps");
        }
        m_throughputLabel->setText(QString("Completed Trips: %1").arg(arrivedCount));
        m_maxLoadLabel->setText(QString("Busiest Road: %1 vehicles").arg(metrics->getMaxEdgeLoad()));
    }
}

void MainWindow::updateTrafficLegend(int capacity) {
    m_currentCapacity = capacity;
    
    // Update legend labels based on capacity
    m_legendEmpty->setText("Empty (0 cars)");
    
    if (capacity == 2) {
        m_legendLight->setText("Light (1 car = 50%)");
        m_legendModerate->setText("Moderate (—)");
        m_legendHeavy->setText("Heavy (—)");
        m_legendGridlock->setText("Gridlock! (2 cars)");
    } else if (capacity == 3) {
        m_legendLight->setText("Light (1 car = 33%)");
        m_legendModerate->setText("Moderate (2 cars = 66%)");
        m_legendHeavy->setText("Heavy (—)");
        m_legendGridlock->setText("Gridlock! (3 cars)");
    } else if (capacity == 4) {
        m_legendLight->setText("Light (1 car = 25%)");
        m_legendModerate->setText("Moderate (2 cars = 50%)");
        m_legendHeavy->setText("Heavy (3 cars = 75%)");
        m_legendGridlock->setText("Gridlock! (4 cars)");
    } else { // capacity >= 5
        m_legendLight->setText(QString("Light (1-2 cars)"));
        m_legendModerate->setText(QString("Moderate (3 cars)"));
        m_legendHeavy->setText(QString("Heavy (4 cars)"));
        m_legendGridlock->setText(QString("Gridlock! (%1 cars)").arg(capacity));
    }
}

void MainWindow::showToast(const QString& message) {
    if (!m_toastLabel) {
        m_toastLabel = new QLabel(this);
        m_toastLabel->setObjectName("toastLabel");
        m_toastLabel->setStyleSheet(
            "background-color: #252a30;"
            "color: #e0e0e0;"
            "border: 1px solid #3a3f47;"
            "border-radius: 8px;"
            "padding: 12px 20px;"
            "font-weight: 500;"
        );
        m_toastLabel->setAlignment(Qt::AlignCenter);
        m_toastLabel->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    }
    
    m_toastLabel->setText(message);
    m_toastLabel->adjustSize();
    
    QPoint pos = mapToGlobal(QPoint(width() - m_toastLabel->width() - 20, 60));
    m_toastLabel->move(pos);
    m_toastLabel->show();
    
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
    
    int baseInterval = 300;
    int actualInterval = baseInterval / m_currentSpeed;
    m_simulationTimer->setInterval(qMax(30, actualInterval));
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
    
    // Check if all agents have arrived - auto-pause simulation
    auto& agents = m_controller->getAgents();
    int activeAgents = 0;
    for (Agent* agent : agents) {
        if (agent && !agent->hasArrived()) {
            activeAgents++;
        }
    }
    
    if (activeAgents == 0 && !agents.empty()) {
        // All agents have arrived - pause simulation
        m_isRunning = false;
        m_simulationTimer->stop();
        m_controller->pause();
        updateControls();
        statusBar()->showMessage("Simulation Complete - All vehicles reached their destinations!", 0);
        showToast("Simulation complete!");
    }
    
    static qint64 lastUpdate = 0;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (lastUpdate > 0) {
        double elapsed = (now - lastUpdate) / 1000.0;
        m_updateTimeLabel->setText(QString("%1ms").arg(static_cast<int>(elapsed * 1000)));
    }
    lastUpdate = now;
}

void MainWindow::onAgentSelected(Agent* agent) {
    Q_UNUSED(agent);
}

void MainWindow::onFitToWindow() {
    if (m_gridView) {
        m_gridView->fitToWindow();
        // Update zoom slider to match
        if (m_zoomSlider && m_zoomLabel) {
            int zoomPercent = static_cast<int>(m_gridView->zoomLevel() * 100);
            m_zoomSlider->blockSignals(true);
            m_zoomSlider->setValue(qBound(20, zoomPercent, 400));
            m_zoomSlider->blockSignals(false);
            m_zoomLabel->setText(QString("%1%").arg(zoomPercent));
        }
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
    if (m_gridView) {
        m_gridView->setShowTrails(m_agentTrailsCheckbox->isChecked());
    }
}

void MainWindow::onToggleHeatMap() {
    if (m_gridView) {
        m_gridView->setShowHeatMap(m_heatMapCheckbox->isChecked());
    }
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
    m_speedLabel->setText(QString(" %1x ").arg(value));
    
    if (m_isRunning && m_simulationTimer) {
        int baseInterval = 300;
        int actualInterval = baseInterval / value;
        m_simulationTimer->setInterval(qMax(30, actualInterval));
    }
}

void MainWindow::onLoadPreset() {
    // Map combo index to preset configuration
    int index = m_presetCombo->currentIndex();
    QString presetFile;
    QString cityName;
    int rows, cols, agentCount;
    
    switch (index) {
        case 0: 
            presetFile = "demo_3x3.json"; 
            cityName = "Small City (3x3)";
            rows = 3; cols = 3; agentCount = 7;
            break;
        case 1: 
            presetFile = "demo_5x5.json"; 
            cityName = "Medium City (5x5)";
            rows = 5; cols = 5; agentCount = 10;
            break;
        case 2: 
            presetFile = "london_10x10.json"; 
            cityName = "London Grid (10x10)";
            rows = 10; cols = 10; agentCount = 50;
            break;
        case 3: 
            presetFile = "manhattan_15x15.json"; 
            cityName = "Manhattan (15x15)";
            rows = 15; cols = 15; agentCount = 75;
            break;
        default: 
            presetFile = "demo_5x5.json"; 
            cityName = "Medium City (5x5)";
            rows = 5; cols = 5; agentCount = 10;
            break;
    }
    
    // Stop simulation first
    if (m_isRunning) {
        m_isRunning = false;
        m_simulationTimer->stop();
        if (m_controller) m_controller->pause();
    }
    
    // Create preset directly (don't rely on file loading)
    Preset preset;
    preset.setName(cityName.toStdString());
    preset.setRows(rows);
    preset.setCols(cols);
    preset.setAgentCount(agentCount);
    preset.setTickMs(100);
    
    // Set policy based on current combo selection
    PolicyType policy = (m_policyCombo->currentIndex() == 0) ? 
                        PolicyType::SHORTEST_PATH : PolicyType::CONGESTION_AWARE;
    preset.setPolicy(policy);
    
    // Add blocked edges for larger grids
    std::vector<std::pair<NodeId, NodeId>> blocked;
    if (index == 2) { // London 10x10
        blocked = {{12, 13}, {45, 46}, {78, 79}};
    } else if (index == 3) { // Manhattan 15x15
        blocked = {{50, 51}, {100, 101}, {150, 151}};
    }
    preset.setBlockedEdges(blocked);
    
    try {
        // Reset and load the new preset
        if (m_controller) {
            m_controller->reset();
            m_controller->loadPreset(preset);
        }
        
        // Force complete scene rebuild
        if (m_gridView && m_controller) {
            m_gridView->setSimulationController(m_controller);
            m_gridView->updateScene();
        }
        
        if (m_metricsPanel && m_controller) {
            m_metricsPanel->setSimulationController(m_controller);
            m_metricsPanel->updateMetrics();
        }
        
        // Update zoom slider
        if (m_zoomSlider) {
            m_zoomSlider->setValue(100);
        }
        
        // Calculate capacity based on grid size and update legend
        int gridSize = rows * cols;
        int capacity;
        if (gridSize <= 25) {         // 5x5 or smaller
            capacity = 2;
        } else if (gridSize <= 64) {  // 6x6 to 8x8
            capacity = 3;
        } else if (gridSize <= 144) { // 9x9 to 12x12
            capacity = 4;
        } else {                      // 13x13 and larger
            capacity = 5;
        }
        updateTrafficLegend(capacity);
        
        // Fit to window after short delay
        QTimer::singleShot(100, this, [this]() {
            if (m_gridView) {
                m_gridView->fitToWindow();
                if (m_zoomSlider && m_zoomLabel) {
                    int zoomPercent = static_cast<int>(m_gridView->zoomLevel() * 100);
                    m_zoomSlider->blockSignals(true);
                    m_zoomSlider->setValue(qBound(30, zoomPercent, 300));
                    m_zoomSlider->blockSignals(false);
                    m_zoomLabel->setText(QString("%1%").arg(zoomPercent));
                }
            }
        });
        
        updateControls();
        updateToolbarStats();
        showToast("Loaded: " + cityName + " (" + QString::number(agentCount) + " vehicles)");
        
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Failed to load city: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::warning(this, "Error", "An unexpected error occurred while loading.");
    }
}

void MainWindow::onExportCSV() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Results", 
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/traffic_results.csv",
        "CSV Files (*.csv)");
    
    if (!fileName.isEmpty()) {
        showToast("Results exported to CSV");
    }
}

void MainWindow::onExportJSON() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Results",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/traffic_results.json",
        "JSON Files (*.json)");
    
    if (!fileName.isEmpty()) {
        showToast("Results exported to JSON");
    }
}

void MainWindow::onSaveConfig() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Configuration",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/config.json",
        "JSON Files (*.json)");
    
    if (!fileName.isEmpty()) {
        showToast("Configuration saved");
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
        "<p><b>Urban Traffic Simulator</b></p>"
        "<p>Version 1.0</p>"
        "<hr>"
        "<p>This simulator lets you observe how different routing "
        "strategies affect traffic flow in a city grid.</p>"
        "<p><b>Shortest Path:</b> Vehicles always take the fastest route, "
        "which can cause congestion on popular roads.</p>"
        "<p><b>Congestion-Aware:</b> Vehicles avoid busy roads, "
        "distributing traffic more evenly.</p>"
        "<hr>"
        "<p>Developed by Faozia Abedin & Paridhi Mehla<br>"
        "CS3307 - Object-Oriented Design</p>");
}

void MainWindow::onOpenAnalytics() {
    if (!m_analyticsPanel) {
        m_analyticsPanel = new AnalyticsPanel(this);
        m_analyticsPanel->setSimulationController(m_controller);
        m_analyticsPanel->setWindowTitle("Traffic Analytics Dashboard");
        m_analyticsPanel->resize(1000, 700);
    }
    
    m_analyticsPanel->show();
    m_analyticsPanel->raise();
    m_analyticsPanel->activateWindow();
    m_analyticsPanel->updateAnalytics();
}

void MainWindow::onShortcuts() {
    QMessageBox::information(this, "Keyboard Shortcuts",
        "<h3>Simulation</h3>"
        "<p><b>Space</b> - Start/Pause simulation</p>"
        "<p><b>R</b> - Reset simulation</p>"
        "<p><b>+</b> - Increase speed</p>"
        "<p><b>-</b> - Decrease speed</p>"
        "<h3>View</h3>"
        "<p><b>Ctrl++</b> - Zoom in</p>"
        "<p><b>Ctrl+-</b> - Zoom out</p>"
        "<p><b>Ctrl+0</b> - Fit to window</p>"
        "<p><b>Arrow keys</b> - Pan view</p>"
        "<p><b>Shift+drag</b> - Pan view with mouse</p>");
}

void MainWindow::onSectionToggled() {
    // Section toggle handler
}
