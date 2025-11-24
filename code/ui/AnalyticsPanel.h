// code/ui/AnalyticsPanel.h
#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QComboBox>
#include <memory>

class QChart;
class QChartView;
class SimulationController;
class TrafficFlowAnalyzer;
class PolicyEffectivenessAnalyzer;
class PredictiveAnalyzer;
class ReportExporter;

/**
 * AnalyticsPanel: Advanced analytics and visualization panel
 * 
 * Features:
 * - Traffic flow analysis (hotspots, flow diagrams, heatmap)
 * - Policy effectiveness metrics (A/B comparison, statistics)
 * - Predictive analytics (trends, predictions, recommendations)
 * - Export functionality (PDF, PowerPoint, CSV, JSON)
 */
class AnalyticsPanel : public QWidget {
    Q_OBJECT

public:
    explicit AnalyticsPanel(QWidget* parent = nullptr);
    ~AnalyticsPanel();
    
    void setSimulationController(SimulationController* controller);
    void updateAnalytics();

private slots:
    void onExportPDF();
    void onExportPowerPoint();
    void onExportCSV();
    void onExportJSON();
    void onRefreshAnalytics();
    void onTabChanged(int index);

private:
    void setupUI();
    void setupTabs();
    void setupFlowAnalysisTab();
    void setupPolicyComparisonTab();
    void setupPredictiveTab();
    void setupExportButtons();
    QWidget* createExportButtonGroup();
    
    void updateFlowAnalysis();
    void updatePolicyComparison();
    void updatePredictiveAnalytics();
    
    void updateHotspotTable();
    void updateUtilizationHeatmap();
    void updateFlowDiagram();
    void updateTimePatterns();
    
    // UI Components
    QTabWidget* m_tabWidget;
    
    // Flow Analysis Tab
    QWidget* m_flowAnalysisTab;
    QTableWidget* m_hotspotTable;
    QLabel* m_heatmapLabel;
    QLabel* m_flowDiagramLabel;
    QLabel* m_timePatternLabel;
    
    // Policy Comparison Tab
    QWidget* m_policyComparisonTab;
    QTableWidget* m_comparisonTable;
    QTextEdit* m_statisticalAnalysis;
    QChartView* m_comparisonChart;
    
    // Predictive Tab
    QWidget* m_predictiveTab;
    QLabel* m_trendProjectionLabel;
    QLabel* m_completionEstimateLabel;
    QLabel* m_bottleneckPredictionLabel;
    QLabel* m_agentRecommendationLabel;
    
    // Export buttons
    QPushButton* m_exportPDFButton;
    QPushButton* m_exportPowerPointButton;
    QPushButton* m_exportCSVButton;
    QPushButton* m_exportJSONButton;
    
    // Data
    SimulationController* m_controller;
    std::unique_ptr<TrafficFlowAnalyzer> m_flowAnalyzer;
    std::unique_ptr<PolicyEffectivenessAnalyzer> m_policyAnalyzer;
    std::unique_ptr<PredictiveAnalyzer> m_predictiveAnalyzer;
    std::unique_ptr<ReportExporter> m_reportExporter;
};

