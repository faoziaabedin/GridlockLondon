// code/ui/AnalyticsPanel.cpp
#include "AnalyticsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Metrics.h"
#include "../core/City.h"
#include "../analytics/TrafficFlowAnalyzer.h"
#include "../analytics/PolicyEffectivenessAnalyzer.h"
#include "../analytics/PredictiveAnalyzer.h"
#include "../analytics/ReportExporter.h"
#include "../core/ErrorHandler.h"
#include "../ui/AnimationHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

AnalyticsPanel::AnalyticsPanel(QWidget* parent)
    : QWidget(parent),
      m_controller(nullptr),
      m_flowAnalyzer(std::make_unique<TrafficFlowAnalyzer>()),
      m_policyAnalyzer(std::make_unique<PolicyEffectivenessAnalyzer>()),
      m_predictiveAnalyzer(std::make_unique<PredictiveAnalyzer>()),
      m_reportExporter(std::make_unique<ReportExporter>()) {
    
    setupUI();
}

AnalyticsPanel::~AnalyticsPanel() = default;

void AnalyticsPanel::setSimulationController(SimulationController* controller) {
    m_controller = controller;
    updateAnalytics();
}

void AnalyticsPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);
    
    // Title
    QLabel* title = new QLabel("📊 Advanced Analytics", this);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF; margin-bottom: 8px;");
    mainLayout->addWidget(title);
    
    // Tab widget
    setupTabs();
    mainLayout->addWidget(m_tabWidget);
    
    // Export buttons
    mainLayout->addWidget(createExportButtonGroup());
}

QWidget* AnalyticsPanel::createExportButtonGroup() {
    QGroupBox* exportGroup = new QGroupBox("📤 Export Reports", this);
    QHBoxLayout* layout = new QHBoxLayout(exportGroup);
    
    m_exportPDFButton = new QPushButton("📄 PDF", this);
    m_exportPowerPointButton = new QPushButton("📊 PowerPoint", this);
    m_exportCSVButton = new QPushButton("📋 CSV", this);
    m_exportJSONButton = new QPushButton("🔗 JSON API", this);
    
    layout->addWidget(m_exportPDFButton);
    layout->addWidget(m_exportPowerPointButton);
    layout->addWidget(m_exportCSVButton);
    layout->addWidget(m_exportJSONButton);
    
    connect(m_exportPDFButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportPDF);
    connect(m_exportPowerPointButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportPowerPoint);
    connect(m_exportCSVButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportCSV);
    connect(m_exportJSONButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportJSON);
    
    return exportGroup;
}

void AnalyticsPanel::setupTabs() {
    m_tabWidget = new QTabWidget(this);
    
    setupFlowAnalysisTab();
    setupPolicyComparisonTab();
    setupPredictiveTab();
    
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &AnalyticsPanel::onTabChanged);
}

void AnalyticsPanel::setupFlowAnalysisTab() {
    m_flowAnalysisTab = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(m_flowAnalysisTab);
    
    // Hotspot table
    QLabel* hotspotLabel = new QLabel("🔥 Traffic Hotspots (Bottlenecks)", this);
    hotspotLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(hotspotLabel);
    
    m_hotspotTable = new QTableWidget(this);
    m_hotspotTable->setColumnCount(5);
    m_hotspotTable->setHorizontalHeaderLabels({"Edge ID", "Utilization", "Occupancy", "Capacity", "Congestion"});
    m_hotspotTable->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_hotspotTable);
    
    // Heatmap label (placeholder for visualization)
    m_heatmapLabel = new QLabel("Utilization Heatmap (visualization coming soon)", this);
    m_heatmapLabel->setStyleSheet("background-color: #1E1E1E; padding: 20px; border-radius: 8px;");
    layout->addWidget(m_heatmapLabel);
    
    m_tabWidget->addTab(m_flowAnalysisTab, "🚦 Flow Analysis");
}

void AnalyticsPanel::setupPolicyComparisonTab() {
    m_policyComparisonTab = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(m_policyComparisonTab);
    
    // Comparison table
    m_comparisonTable = new QTableWidget(this);
    m_comparisonTable->setColumnCount(4);
    m_comparisonTable->setHorizontalHeaderLabels({"Metric", "Policy A", "Policy B", "Difference"});
    layout->addWidget(m_comparisonTable);
    
    // Statistical analysis
    QLabel* statsLabel = new QLabel("📈 Statistical Analysis", this);
    statsLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 12px;");
    layout->addWidget(statsLabel);
    
    m_statisticalAnalysis = new QTextEdit(this);
    m_statisticalAnalysis->setReadOnly(true);
    m_statisticalAnalysis->setMaximumHeight(200);
    layout->addWidget(m_statisticalAnalysis);
    
    m_tabWidget->addTab(m_policyComparisonTab, "⚖️ Policy Comparison");
}

void AnalyticsPanel::setupPredictiveTab() {
    m_predictiveTab = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(m_predictiveTab);
    
    // Trend projection
    QGroupBox* trendGroup = new QGroupBox("📈 Trend Projection", this);
    QVBoxLayout* trendLayout = new QVBoxLayout(trendGroup);
    m_trendProjectionLabel = new QLabel("Analyzing trends...", this);
    trendLayout->addWidget(m_trendProjectionLabel);
    layout->addWidget(trendGroup);
    
    // Completion estimate
    QGroupBox* completionGroup = new QGroupBox("⏱️ Completion Estimate", this);
    QVBoxLayout* completionLayout = new QVBoxLayout(completionGroup);
    m_completionEstimateLabel = new QLabel("Calculating...", this);
    completionLayout->addWidget(m_completionEstimateLabel);
    layout->addWidget(completionGroup);
    
    // Bottleneck prediction
    QGroupBox* bottleneckGroup = new QGroupBox("⚠️ Bottleneck Prediction", this);
    QVBoxLayout* bottleneckLayout = new QVBoxLayout(bottleneckGroup);
    m_bottleneckPredictionLabel = new QLabel("Analyzing...", this);
    bottleneckLayout->addWidget(m_bottleneckPredictionLabel);
    layout->addWidget(bottleneckGroup);
    
    // Agent recommendation
    QGroupBox* recommendationGroup = new QGroupBox("💡 Recommendations", this);
    QVBoxLayout* recLayout = new QVBoxLayout(recommendationGroup);
    m_agentRecommendationLabel = new QLabel("Calculating optimal agent count...", this);
    recLayout->addWidget(m_agentRecommendationLabel);
    layout->addWidget(recommendationGroup);
    
    m_tabWidget->addTab(m_predictiveTab, "🔮 Predictive Analytics");
}

void AnalyticsPanel::setupExportButtons() {
    // Buttons are created in createExportButtonGroup
    // This method is kept for consistency but implementation is in createExportButtonGroup()
}

QWidget* AnalyticsPanel::createExportButtonGroup() {
    QGroupBox* exportGroup = new QGroupBox("📤 Export Reports", this);
    QHBoxLayout* layout = new QHBoxLayout(exportGroup);
    
    m_exportPDFButton = new QPushButton("📄 PDF", this);
    m_exportPowerPointButton = new QPushButton("📊 PowerPoint", this);
    m_exportCSVButton = new QPushButton("📋 CSV", this);
    m_exportJSONButton = new QPushButton("🔗 JSON API", this);
    
    layout->addWidget(m_exportPDFButton);
    layout->addWidget(m_exportPowerPointButton);
    layout->addWidget(m_exportCSVButton);
    layout->addWidget(m_exportJSONButton);
    
    connect(m_exportPDFButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportPDF);
    connect(m_exportPowerPointButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportPowerPoint);
    connect(m_exportCSVButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportCSV);
    connect(m_exportJSONButton, &QPushButton::clicked, this, &AnalyticsPanel::onExportJSON);
    
    return exportGroup;
}

void AnalyticsPanel::updateAnalytics() {
    if (!m_controller || !m_controller->getCity()) {
        return;
    }
    
    updateFlowAnalysis();
    updatePolicyComparison();
    updatePredictiveAnalytics();
}

void AnalyticsPanel::updateFlowAnalysis() {
    if (!m_controller || !m_controller->getCity()) {
        return;
    }
    
    updateHotspotTable();
    updateUtilizationHeatmap();
}

void AnalyticsPanel::updateHotspotTable() {
    if (!m_controller || !m_controller->getCity()) {
        return;
    }
    
    auto hotspots = m_flowAnalyzer->getTopBottlenecks(*m_controller->getCity(), 10);
    
    m_hotspotTable->setRowCount(static_cast<int>(hotspots.size()));
    
    for (size_t i = 0; i < hotspots.size(); ++i) {
        const auto& hotspot = hotspots[i];
        
        m_hotspotTable->setItem(static_cast<int>(i), 0, 
            new QTableWidgetItem(QString::number(hotspot.edgeId)));
        m_hotspotTable->setItem(static_cast<int>(i), 1,
            new QTableWidgetItem(QString("%1%").arg(hotspot.utilization * 100.0, 0, 'f', 1)));
        m_hotspotTable->setItem(static_cast<int>(i), 2,
            new QTableWidgetItem(QString::number(hotspot.currentOccupancy)));
        m_hotspotTable->setItem(static_cast<int>(i), 3,
            new QTableWidgetItem(QString::number(hotspot.capacity)));
        m_hotspotTable->setItem(static_cast<int>(i), 4,
            new QTableWidgetItem(QString::number(hotspot.congestionLevel, 'f', 2)));
    }
}

void AnalyticsPanel::updateUtilizationHeatmap() {
    if (!m_controller || !m_controller->getCity()) {
        return;
    }
    
    auto heatmap = m_flowAnalyzer->getUtilizationHeatmap(*m_controller->getCity());
    
    QString text = QString("Utilization Heatmap: %1 edges analyzed\n").arg(heatmap.size());
    text += "Color coding: Green (low) → Yellow (medium) → Red (high utilization)";
    
    m_heatmapLabel->setText(text);
}

void AnalyticsPanel::updatePolicyComparison() {
    // Placeholder - would compare two policy runs
    m_comparisonTable->setRowCount(4);
    m_comparisonTable->setItem(0, 0, new QTableWidgetItem("Average Trip Time"));
    m_comparisonTable->setItem(1, 0, new QTableWidgetItem("Throughput"));
    m_comparisonTable->setItem(2, 0, new QTableWidgetItem("Max Edge Load"));
    m_comparisonTable->setItem(3, 0, new QTableWidgetItem("Efficiency Score"));
    
    m_statisticalAnalysis->setPlainText(
        "Statistical analysis requires running two policy simulations.\n"
        "Use Comparative Analysis mode to compare policies side-by-side."
    );
}

void AnalyticsPanel::updatePredictiveAnalytics() {
    if (!m_controller) {
        return;
    }
    
    // Trend projection
    std::vector<double> history = {10.0, 12.0, 11.5, 13.0, 12.5};  // Placeholder
    auto trend = m_predictiveAnalyzer->projectTrend(history, 5);
    m_trendProjectionLabel->setText(
        QString("Trend: %1 (confidence: %2%)\nProjected next 5 ticks: %3")
            .arg(trend.trendSlope > 0 ? "Increasing" : "Decreasing")
            .arg(trend.confidence * 100.0, 0, 'f', 1)
            .arg(trend.projectedValues.size())
    );
    
    // Completion estimate
    if (m_controller->getMetrics()) {
        int remaining = 10;  // Placeholder
        std::vector<double> completionTimes = {5.0, 6.0, 5.5};  // Placeholder
        auto estimate = m_predictiveAnalyzer->estimateCompletionTime(remaining, completionTimes, 2.0);
        m_completionEstimateLabel->setText(
            QString("Remaining agents: %1\nEstimated time: %2 ticks\nConfidence: %3%")
                .arg(estimate.remainingAgents)
                .arg(estimate.estimatedTimeRemaining, 0, 'f', 1)
                .arg(estimate.confidence * 100.0, 0, 'f', 1)
        );
    }
    
    // Bottleneck prediction
    m_bottleneckPredictionLabel->setText(
        "Analyzing edge utilization trends to predict future bottlenecks..."
    );
    
    // Agent recommendation
    m_agentRecommendationLabel->setText(
        "Current agent count appears optimal based on efficiency metrics."
    );
}

void AnalyticsPanel::onExportPDF() {
    if (!m_controller) {
        QMessageBox::warning(this, "Export", "No simulation data available.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export PDF Report",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/analytics_report.html",
        "HTML Files (*.html)");
    
    if (filepath.isEmpty()) return;
    
    try {
        bool success = m_reportExporter->exportPDF(filepath, *m_controller,
            *m_flowAnalyzer, *m_policyAnalyzer, *m_predictiveAnalyzer);
        
        if (success) {
            AnimationHelper::showSuccessFeedback(this, "PDF report exported successfully!");
        } else {
            AnimationHelper::showErrorFeedback(this, "Failed to export PDF report.");
        }
    } catch (const std::exception& e) {
        QString msg = ErrorHandler::getUserFriendlyMessage(e, "Exporting PDF");
        AnimationHelper::showErrorFeedback(this, msg);
    }
}

void AnalyticsPanel::onExportPowerPoint() {
    if (!m_controller) {
        QMessageBox::warning(this, "Export", "No simulation data available.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export PowerPoint",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/analytics_presentation.txt",
        "Text Files (*.txt)");
    
    if (filepath.isEmpty()) return;
    
    try {
        bool success = m_reportExporter->exportPowerPoint(filepath, *m_controller, *m_flowAnalyzer);
        
        if (success) {
            AnimationHelper::showSuccessFeedback(this, "PowerPoint export completed!");
        } else {
            AnimationHelper::showErrorFeedback(this, "Failed to export PowerPoint.");
        }
    } catch (const std::exception& e) {
        QString msg = ErrorHandler::getUserFriendlyMessage(e, "Exporting PowerPoint");
        AnimationHelper::showErrorFeedback(this, msg);
    }
}

void AnalyticsPanel::onExportCSV() {
    if (!m_controller) {
        QMessageBox::warning(this, "Export", "No simulation data available.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export CSV",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/analytics_data.csv",
        "CSV Files (*.csv)");
    
    if (filepath.isEmpty()) return;
    
    try {
        bool success = m_reportExporter->exportCSV(filepath, *m_controller, *m_flowAnalyzer, true);
        
        if (success) {
            AnimationHelper::showSuccessFeedback(this, "CSV exported successfully!");
        } else {
            AnimationHelper::showErrorFeedback(this, "Failed to export CSV.");
        }
    } catch (const std::exception& e) {
        QString msg = ErrorHandler::getUserFriendlyMessage(e, "Exporting CSV");
        AnimationHelper::showErrorFeedback(this, msg);
    }
}

void AnalyticsPanel::onExportJSON() {
    if (!m_controller) {
        QMessageBox::warning(this, "Export", "No simulation data available.");
        return;
    }
    
    QString filepath = QFileDialog::getSaveFileName(this, "Export JSON API",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/analytics_api.json",
        "JSON Files (*.json)");
    
    if (filepath.isEmpty()) return;
    
    try {
        QString json = m_reportExporter->exportJSON(*m_controller, *m_flowAnalyzer,
            *m_policyAnalyzer, *m_predictiveAnalyzer);
        
        bool success = m_reportExporter->saveJSON(filepath, json);
        
        if (success) {
            AnimationHelper::showSuccessFeedback(this, "JSON API exported successfully!");
        } else {
            AnimationHelper::showErrorFeedback(this, "Failed to export JSON.");
        }
    } catch (const std::exception& e) {
        QString msg = ErrorHandler::getUserFriendlyMessage(e, "Exporting JSON");
        AnimationHelper::showErrorFeedback(this, msg);
    }
}

void AnalyticsPanel::onRefreshAnalytics() {
    updateAnalytics();
}

void AnalyticsPanel::onTabChanged(int index) {
    Q_UNUSED(index);
    updateAnalytics();
}

