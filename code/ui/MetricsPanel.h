#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTableWidget>
#include <QPushButton>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

// Include Qt Charts headers (needed for member variables)
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>

class SimulationController;
class Metrics;

/**
 * Professional metrics panel with real-time animated charts.
 * Features: Stat cards, line chart, bar chart, heat map, comparison table.
 */
class MetricsPanel : public QWidget {
    Q_OBJECT

public:
    explicit MetricsPanel(QWidget* parent = nullptr);
    ~MetricsPanel();
    
    void setSimulationController(SimulationController* controller);
    void updateMetrics();

private slots:
    void onExportImage();
    void onCopyToClipboard();
    void onShareChart();
    void animateStatCard(QLabel* label, double oldValue, double newValue);

private:
    void setupUI();
    void setupStatCards();
    void setupCharts();
    void setupComparisonTable();
    void updateStatCards(double avgTime, int completed, int maxLoad);
    void updateCharts(double avgTime, int completed, int maxLoad);
    void updateComparisonTable();
    void applyChartTheme(QChart* chart);
    QColor getGradientColor(double value, double min, double max) const;
    
    SimulationController* m_controller;
    
    // Stat Cards
    QWidget* m_statCardsWidget;
    QLabel* m_avgTimeCard;
    QLabel* m_avgTimeValue;
    QLabel* m_avgTimeTrend;
    QLabel* m_completedCard;
    QLabel* m_completedValue;
    QLabel* m_completedTrend;
    QLabel* m_maxLoadCard;
    QLabel* m_maxLoadValue;
    QLabel* m_maxLoadTrend;
    
    // Charts
    QChartView* m_tripTimeChartView;
    QChart* m_tripTimeChart;
    QLineSeries* m_tripTimeSeries;
    QValueAxis* m_tripTimeAxisX;
    QValueAxis* m_tripTimeAxisY;
    
    QChartView* m_throughputChartView;
    QChart* m_throughputChart;
    QBarSeries* m_throughputSeries;
    QBarCategoryAxis* m_throughputAxisX;
    QValueAxis* m_throughputAxisY;
    
    QChartView* m_congestionChartView;
    QChart* m_congestionChart;
    QPieSeries* m_congestionSeries;
    
    // Comparison Table
    QTableWidget* m_comparisonTable;
    
    // Export buttons
    QPushButton* m_exportImageButton;
    QPushButton* m_copyButton;
    QPushButton* m_shareButton;
    
    // Data
    QVector<QPointF> m_tripTimeData;
    QVector<int> m_throughputData;
    QVector<int> m_congestionLevels; // 0-5 levels
    static constexpr int MAX_HISTORY = 100;
    static constexpr int INTERVAL_SIZE = 10; // For throughput intervals
    
    // Previous values for trend calculation
    double m_prevAvgTime;
    int m_prevCompleted;
    int m_prevMaxLoad;
    
    // Policy comparison data
    double m_shortestPathAvgTime;
    double m_congestionAwareAvgTime;
    int m_shortestPathThroughput;
    int m_congestionAwareThroughput;
    int m_shortestPathMaxLoad;
    int m_congestionAwareMaxLoad;
    
    // Animation
    QTimer* m_animationTimer;
    QMap<QLabel*, QPropertyAnimation*> m_counterAnimations;
};
