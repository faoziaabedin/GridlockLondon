#include "MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Metrics.h"
#include "../core/Agent.h"
#include "../core/Preset.h"
#include "../core/Types.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStandardPaths>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include <QtCharts/QLegend>
#include <QtCharts/QAreaSeries>
#include <algorithm>
#include <QtMath>

QT_USE_NAMESPACE

MetricsPanel::MetricsPanel(QWidget* parent)
    : QWidget(parent), m_controller(nullptr),
      m_prevAvgTime(0.0), m_prevCompleted(0), m_prevMaxLoad(0),
      m_shortestPathAvgTime(0.0), m_congestionAwareAvgTime(0.0),
      m_shortestPathThroughput(0), m_congestionAwareThroughput(0),
      m_shortestPathMaxLoad(0), m_congestionAwareMaxLoad(0),
      m_animationTimer(new QTimer(this)) {
    
    setObjectName("metricsPanel");
    setupUI();
    
    // Animation timer for smooth updates
    m_animationTimer->setInterval(16); // ~60 FPS
    connect(m_animationTimer, &QTimer::timeout, this, [this]() {
        // Update animations
    });
    m_animationTimer->start();
}

MetricsPanel::~MetricsPanel() = default;

void MetricsPanel::setSimulationController(SimulationController* controller) {
    m_controller = controller;
    updateMetrics();
}

void MetricsPanel::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(16);
    
    // Title
    QLabel* titleLabel = new QLabel("📊 METRICS", this);
    titleLabel->setObjectName("sectionTitle");
    layout->addWidget(titleLabel);
    
    // Stat Cards
    setupStatCards();
    layout->addWidget(m_statCardsWidget);
    
    // Charts
    setupCharts();
    layout->addWidget(m_tripTimeChartView);
    layout->addWidget(m_throughputChartView);
    layout->addWidget(m_congestionChartView);
    
    // Comparison Table
    setupComparisonTable();
    layout->addWidget(m_comparisonTable);
    
    // Export buttons
    QHBoxLayout* exportLayout = new QHBoxLayout();
    exportLayout->setSpacing(8);
    
    m_exportImageButton = new QPushButton("💾 Save Chart as Image", this);
    m_exportImageButton->setObjectName("secondaryButton");
    connect(m_exportImageButton, &QPushButton::clicked, this, &MetricsPanel::onExportImage);
    exportLayout->addWidget(m_exportImageButton);
    
    m_copyButton = new QPushButton("📋 Copy to Clipboard", this);
    m_copyButton->setObjectName("secondaryButton");
    connect(m_copyButton, &QPushButton::clicked, this, &MetricsPanel::onCopyToClipboard);
    exportLayout->addWidget(m_copyButton);
    
    m_shareButton = new QPushButton("🔗 Share", this);
    m_shareButton->setObjectName("secondaryButton");
    connect(m_shareButton, &QPushButton::clicked, this, &MetricsPanel::onShareChart);
    exportLayout->addWidget(m_shareButton);
    
    layout->addLayout(exportLayout);
    layout->addStretch();
}

void MetricsPanel::setupStatCards() {
    m_statCardsWidget = new QWidget(this);
    QHBoxLayout* cardsLayout = new QHBoxLayout(m_statCardsWidget);
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    cardsLayout->setSpacing(12);
    
    // Card 1: Avg Time
    QWidget* card1 = new QWidget(this);
    card1->setObjectName("statCard");
    QVBoxLayout* card1Layout = new QVBoxLayout(card1);
    card1Layout->setContentsMargins(16, 12, 16, 12);
    card1Layout->setSpacing(4);
    
    m_avgTimeCard = new QLabel("📊 Avg Time", card1);
    m_avgTimeCard->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    card1Layout->addWidget(m_avgTimeCard);
    
    m_avgTimeValue = new QLabel("0.0s", card1);
    m_avgTimeValue->setStyleSheet("color: #FFFFFF; font-size: 32px; font-weight: bold;");
    card1Layout->addWidget(m_avgTimeValue);
    
    m_avgTimeTrend = new QLabel("—", card1);
    m_avgTimeTrend->setStyleSheet("color: #CCCCCC; font-size: 11px;");
    card1Layout->addWidget(m_avgTimeTrend);
    
    cardsLayout->addWidget(card1);
    
    // Card 2: Completed
    QWidget* card2 = new QWidget(this);
    card2->setObjectName("statCard");
    QVBoxLayout* card2Layout = new QVBoxLayout(card2);
    card2Layout->setContentsMargins(16, 12, 16, 12);
    card2Layout->setSpacing(4);
    
    m_completedCard = new QLabel("🚗 Completed", card2);
    m_completedCard->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    card2Layout->addWidget(m_completedCard);
    
    m_completedValue = new QLabel("0", card2);
    m_completedValue->setStyleSheet("color: #FFFFFF; font-size: 32px; font-weight: bold;");
    card2Layout->addWidget(m_completedValue);
    
    m_completedTrend = new QLabel("—", card2);
    m_completedTrend->setStyleSheet("color: #CCCCCC; font-size: 11px;");
    card2Layout->addWidget(m_completedTrend);
    
    cardsLayout->addWidget(card2);
    
    // Card 3: Max Load
    QWidget* card3 = new QWidget(this);
    card3->setObjectName("statCard");
    QVBoxLayout* card3Layout = new QVBoxLayout(card3);
    card3Layout->setContentsMargins(16, 12, 16, 12);
    card3Layout->setSpacing(4);
    
    m_maxLoadCard = new QLabel("🚦 Max Load", card3);
    m_maxLoadCard->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    card3Layout->addWidget(m_maxLoadCard);
    
    m_maxLoadValue = new QLabel("0", card3);
    m_maxLoadValue->setStyleSheet("color: #FFFFFF; font-size: 32px; font-weight: bold;");
    card3Layout->addWidget(m_maxLoadValue);
    
    m_maxLoadTrend = new QLabel("—", card3);
    m_maxLoadTrend->setStyleSheet("color: #CCCCCC; font-size: 11px;");
    card3Layout->addWidget(m_maxLoadTrend);
    
    cardsLayout->addWidget(card3);
}

void MetricsPanel::setupCharts() {
    // Trip Time Line Chart
    m_tripTimeChart = new QChart();
    m_tripTimeChart->setTitle("Trip Time Over Simulation");
    m_tripTimeChart->setTitleBrush(QBrush(QColor(204, 204, 204)));
    m_tripTimeChart->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_tripTimeChart->setPlotAreaBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_tripTimeChart->setPlotAreaBackgroundVisible(true);
    
    m_tripTimeSeries = new QLineSeries();
    m_tripTimeSeries->setName("Trip Time");
    m_tripTimeSeries->setPen(QPen(QColor(6, 182, 212), 2)); // Cyan
    m_tripTimeSeries->setBrush(QBrush(QColor(6, 182, 212, 50))); // Semi-transparent fill
    
    // Create area series for gradient fill
    QAreaSeries* areaSeries = new QAreaSeries(m_tripTimeSeries);
    areaSeries->setColor(QColor(6, 182, 212));
    areaSeries->setBorderColor(QColor(6, 182, 212));
    
    m_tripTimeChart->addSeries(areaSeries);
    
    m_tripTimeAxisX = new QValueAxis();
    m_tripTimeAxisX->setTitleText("Tick");
    m_tripTimeAxisX->setTitleBrush(QBrush(QColor(204, 204, 204)));
    m_tripTimeAxisX->setLabelsColor(QColor(204, 204, 204));
    m_tripTimeAxisX->setGridLineColor(QColor(62, 62, 66));
    m_tripTimeAxisX->setLinePenColor(QColor(62, 62, 66));
    m_tripTimeAxisX->setRange(0, 100);
    
    m_tripTimeAxisY = new QValueAxis();
    m_tripTimeAxisY->setTitleText("Time (steps)");
    m_tripTimeAxisY->setTitleBrush(QBrush(QColor(204, 204, 204)));
    m_tripTimeAxisY->setLabelsColor(QColor(204, 204, 204));
    m_tripTimeAxisY->setGridLineColor(QColor(62, 62, 66));
    m_tripTimeAxisY->setLinePenColor(QColor(62, 62, 66));
    m_tripTimeAxisY->setRange(0, 20);
    
    m_tripTimeChart->addAxis(m_tripTimeAxisX, Qt::AlignBottom);
    m_tripTimeChart->addAxis(m_tripTimeAxisY, Qt::AlignLeft);
    areaSeries->attachAxis(m_tripTimeAxisX);
    areaSeries->attachAxis(m_tripTimeAxisY);
    
    m_tripTimeChartView = new QChartView(m_tripTimeChart, this);
    m_tripTimeChartView->setRenderHint(QPainter::Antialiasing);
    m_tripTimeChartView->setMinimumHeight(200);
    m_tripTimeChartView->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    applyChartTheme(m_tripTimeChart);
    
    // Throughput Bar Chart
    m_throughputChart = new QChart();
    m_throughputChart->setTitle("Agents Completed Per Interval");
    m_throughputChart->setTitleBrush(QBrush(QColor(204, 204, 204)));
    m_throughputChart->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_throughputChart->setPlotAreaBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_throughputChart->setPlotAreaBackgroundVisible(true);
    
    m_throughputSeries = new QBarSeries();
    m_throughputChart->addSeries(m_throughputSeries);
    
    m_throughputAxisX = new QBarCategoryAxis();
    m_throughputAxisX->setTitleText("Interval");
    m_throughputAxisX->setTitleBrush(QBrush(QColor(204, 204, 204)));
    m_throughputAxisX->setLabelsColor(QColor(204, 204, 204));
    m_throughputAxisX->setGridLineColor(QColor(62, 62, 66));
    m_throughputAxisX->setLinePenColor(QColor(62, 62, 66));
    
    m_throughputAxisY = new QValueAxis();
    m_throughputAxisY->setTitleText("Agents");
    m_throughputAxisY->setTitleBrush(QBrush(QColor(204, 204, 204)));
    m_throughputAxisY->setLabelsColor(QColor(204, 204, 204));
    m_throughputAxisY->setGridLineColor(QColor(62, 62, 66));
    m_throughputAxisY->setLinePenColor(QColor(62, 62, 66));
    m_throughputAxisY->setRange(0, 10);
    
    m_throughputChart->addAxis(m_throughputAxisX, Qt::AlignBottom);
    m_throughputChart->addAxis(m_throughputAxisY, Qt::AlignLeft);
    m_throughputSeries->attachAxis(m_throughputAxisX);
    m_throughputSeries->attachAxis(m_throughputAxisY);
    
    m_throughputChartView = new QChartView(m_throughputChart, this);
    m_throughputChartView->setRenderHint(QPainter::Antialiasing);
    m_throughputChartView->setMinimumHeight(200);
    m_throughputChartView->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    applyChartTheme(m_throughputChart);
    
    // Congestion Heat Map (Radial/Donut Chart)
    m_congestionChart = new QChart();
    m_congestionChart->setTitle("Max Edge Load Over Time");
    m_congestionChart->setTitleBrush(QBrush(QColor(204, 204, 204)));
    m_congestionChart->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_congestionChart->setPlotAreaBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_congestionChart->setPlotAreaBackgroundVisible(true);
    
    m_congestionSeries = new QPieSeries();
    m_congestionSeries->setHoleSize(0.5); // Donut chart
    m_congestionChart->addSeries(m_congestionSeries);
    
    m_congestionChartView = new QChartView(m_congestionChart, this);
    m_congestionChartView->setRenderHint(QPainter::Antialiasing);
    m_congestionChartView->setMinimumHeight(200);
    m_congestionChartView->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    applyChartTheme(m_congestionChart);
}

void MetricsPanel::setupComparisonTable() {
    m_comparisonTable = new QTableWidget(4, 3, this);
    m_comparisonTable->setHorizontalHeaderLabels(QStringList() << "Metric" << "Shortest Path" << "Congestion-Aware");
    m_comparisonTable->verticalHeader()->setVisible(false);
    m_comparisonTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_comparisonTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_comparisonTable->setAlternatingRowColors(true);
    m_comparisonTable->setStyleSheet(
        "QTableWidget {"
        "background-color: #1E1E1E;"
        "color: #CCCCCC;"
        "border: 1px solid #3E3E42;"
        "border-radius: 8px;"
        "gridline-color: #3E3E42;"
        "}"
        "QTableWidget::item {"
        "padding: 8px;"
        "}"
        "QTableWidget::item:selected {"
        "background-color: #007ACC;"
        "}"
        "QHeaderView::section {"
        "background-color: #252526;"
        "color: #CCCCCC;"
        "padding: 8px;"
        "border: none;"
        "border-bottom: 1px solid #3E3E42;"
        "font-weight: 600;"
        "}"
    );
    
    QStringList rowLabels = {"Avg Trip Time", "Total Throughput", "Max Edge Load", "Efficiency Score"};
    for (int i = 0; i < 4; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(rowLabels[i]);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_comparisonTable->setItem(i, 0, item);
        
        for (int j = 1; j < 3; ++j) {
            QTableWidgetItem* valueItem = new QTableWidgetItem("—");
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
            valueItem->setTextAlignment(Qt::AlignCenter);
            m_comparisonTable->setItem(i, j, valueItem);
        }
    }
    
    m_comparisonTable->resizeColumnsToContents();
}

void MetricsPanel::applyChartTheme(QChart* chart) {
    chart->setTheme(QChart::ChartThemeDark);
    
    // Customize legend
    QLegend* legend = chart->legend();
    if (legend) {
        legend->setLabelColor(QColor(204, 204, 204));
        legend->setBackgroundVisible(false);
        legend->setBorderColor(QColor(62, 62, 66));
    }
}

void MetricsPanel::updateMetrics() {
    if (!m_controller) {
        return;
    }
    
    Metrics* metrics = m_controller->getMetrics();
    auto& agents = m_controller->getAgents();
    
    if (!metrics) {
        return;
    }
    
    // Calculate metrics
    int activeCount = 0;
    int arrivedCount = 0;
    int totalTravelTime = 0;
    int maxLoad = metrics->getMaxEdgeLoad();
    
    for (Agent* agent : agents) {
        if (!agent) continue;
        if (agent->hasArrived()) {
            arrivedCount++;
            totalTravelTime += agent->getTravelTime();
        } else {
            activeCount++;
        }
    }
    
    double avgTravelTime = arrivedCount > 0 ? static_cast<double>(totalTravelTime) / arrivedCount : 0.0;
    
    // Update stat cards
    updateStatCards(avgTravelTime, arrivedCount, maxLoad);
    
    // Update charts
    updateCharts(avgTravelTime, arrivedCount, maxLoad);
    
    // Update comparison table
    updateComparisonTable();
}

void MetricsPanel::updateStatCards(double avgTime, int completed, int maxLoad) {
    // Animate counter for avg time
    if (m_prevAvgTime != avgTime) {
        animateStatCard(m_avgTimeValue, m_prevAvgTime, avgTime);
        m_avgTimeValue->setText(QString("%1s").arg(avgTime, 0, 'f', 1));
        
        // Calculate trend
        if (m_prevAvgTime > 0) {
            double change = ((avgTime - m_prevAvgTime) / m_prevAvgTime) * 100.0;
            QString trend = change > 0 ? QString("⬆ %1%").arg(qAbs(change), 0, 'f', 0) :
                           change < 0 ? QString("⬇ %1%").arg(qAbs(change), 0, 'f', 0) : "—";
            m_avgTimeTrend->setText(trend);
            m_avgTimeTrend->setStyleSheet(change < 0 ? "color: #4ade80;" : 
                                         change > 0 ? "color: #ef4444;" : 
                                         "color: #CCCCCC;");
        }
        m_prevAvgTime = avgTime;
    }
    
    // Animate counter for completed
    if (m_prevCompleted != completed) {
        animateStatCard(m_completedValue, m_prevCompleted, completed);
        m_completedValue->setText(QString::number(completed));
        
        // Calculate trend
        if (m_prevCompleted > 0) {
            double change = ((completed - m_prevCompleted) / static_cast<double>(m_prevCompleted)) * 100.0;
            QString trend = change > 0 ? QString("⬆ %1%").arg(qAbs(change), 0, 'f', 0) : "—";
            m_completedTrend->setText(trend);
            m_completedTrend->setStyleSheet("color: #4ade80;");
        } else if (completed > 0) {
            m_completedTrend->setText("⬆ 100%");
            m_completedTrend->setStyleSheet("color: #4ade80;");
        }
        m_prevCompleted = completed;
    }
    
    // Animate counter for max load
    if (m_prevMaxLoad != maxLoad) {
        animateStatCard(m_maxLoadValue, m_prevMaxLoad, maxLoad);
        m_maxLoadValue->setText(QString::number(maxLoad));
        
        // Calculate trend
        if (m_prevMaxLoad > 0) {
            double change = ((maxLoad - m_prevMaxLoad) / static_cast<double>(m_prevMaxLoad)) * 100.0;
            QString trend = change > 0 ? QString("⬆ %1%").arg(qAbs(change), 0, 'f', 0) :
                           change < 0 ? QString("⬇ %1%").arg(qAbs(change), 0, 'f', 0) : "—";
            m_maxLoadTrend->setText(trend);
            m_maxLoadTrend->setStyleSheet(change < 0 ? "color: #4ade80;" : 
                                         change > 0 ? "color: #ef4444;" : 
                                         "color: #CCCCCC;");
        }
        m_prevMaxLoad = maxLoad;
    }
}

void MetricsPanel::updateCharts(double avgTime, int completed, int maxLoad) {
    // Update Trip Time Line Chart
    int currentTick = m_controller->getMetrics()->getCurrentTick();
    m_tripTimeData.append(QPointF(currentTick, avgTime));
    
    if (m_tripTimeData.size() > MAX_HISTORY) {
        m_tripTimeData.removeFirst();
    }
    
    m_tripTimeSeries->clear();
    for (const QPointF& point : m_tripTimeData) {
        m_tripTimeSeries->append(point);
    }
    
    // Update axis ranges
    if (!m_tripTimeData.isEmpty()) {
        double minX = m_tripTimeData.first().x();
        double maxX = m_tripTimeData.last().x();
        double minY = 0;
        double maxY = 20;
        
        for (const QPointF& point : m_tripTimeData) {
            minY = qMin(minY, point.y());
            maxY = qMax(maxY, point.y());
        }
        
        if (maxY > minY) {
            m_tripTimeAxisY->setRange(0, maxY * 1.1);
        }
        m_tripTimeAxisX->setRange(qMax(0.0, maxX - 100), maxX);
    }
    
    // Update Throughput Bar Chart
    static int intervalCounter = 0;
    intervalCounter++;
    
    if (intervalCounter >= INTERVAL_SIZE) {
        m_throughputData.append(completed);
        intervalCounter = 0;
        
        if (m_throughputData.size() > 10) {
            m_throughputData.removeFirst();
        }
        
        // Update bar series
        m_throughputSeries->clear();
        QStringList categories;
        
        for (int i = 0; i < m_throughputData.size(); ++i) {
            QBarSet* barSet = new QBarSet(QString::number(i + 1));
            barSet->append(m_throughputData[i]);
            
            // Color based on value
            QColor barColor = getGradientColor(m_throughputData[i], 0, 10);
            barSet->setColor(barColor);
            
            m_throughputSeries->append(barSet);
            categories << QString::number(i + 1);
        }
        
        m_throughputAxisX->setCategories(categories);
        
        if (!m_throughputData.isEmpty()) {
            int maxVal = *std::max_element(m_throughputData.begin(), m_throughputData.end());
            m_throughputAxisY->setRange(0, qMax(10, maxVal + 2));
        }
    }
    
    // Update Congestion Heat Map
    // Categorize max load into levels (0-5)
    int level = qBound(0, maxLoad, 5);
    m_congestionLevels.append(level);
    
    if (m_congestionLevels.size() > 20) {
        m_congestionLevels.removeFirst();
    }
    
    // Count occurrences of each level
    QVector<int> levelCounts(6, 0);
    for (int l : m_congestionLevels) {
        levelCounts[l]++;
    }
    
    m_congestionSeries->clear();
    
    QVector<QColor> levelColors = {
        QColor(59, 130, 246),   // Level 1: Blue
        QColor(6, 182, 212),    // Level 2: Cyan
        QColor(245, 158, 11),   // Level 3: Yellow
        QColor(249, 115, 22),   // Level 4: Orange
        QColor(239, 68, 68),    // Level 5: Red
        QColor(139, 0, 0)       // Level 6: Dark Red
    };
    
    QStringList levelLabels = {"Level 1", "Level 2", "Level 3", "Level 4", "Level 5", "Level 6"};
    
    for (int i = 0; i < 6; ++i) {
        if (levelCounts[i] > 0) {
            QPieSlice* slice = m_congestionSeries->append(
                QString("%1 (%2)").arg(levelLabels[i]).arg(levelCounts[i]),
                levelCounts[i]
            );
            slice->setColor(levelColors[i]);
            slice->setLabelVisible(true);
            slice->setLabelColor(QColor(204, 204, 204));
        }
    }
}

void MetricsPanel::updateComparisonTable() {
    if (!m_controller) return;
    
    // Calculate comparison metrics
    auto& agents = m_controller->getAgents();
    PolicyType currentPolicy = m_controller->getPolicy();
    
    int arrivedCount = 0;
    int totalTime = 0;
    int maxLoad = m_controller->getMetrics()->getMaxEdgeLoad();
    
    for (Agent* agent : agents) {
        if (agent && agent->hasArrived()) {
            arrivedCount++;
            totalTime += agent->getTravelTime();
        }
    }
    
    double avgTime = arrivedCount > 0 ? static_cast<double>(totalTime) / arrivedCount : 0.0;
    
    if (currentPolicy == PolicyType::SHORTEST_PATH) {
        m_shortestPathAvgTime = avgTime;
        m_shortestPathThroughput = arrivedCount;
        m_shortestPathMaxLoad = maxLoad;
    } else {
        m_congestionAwareAvgTime = avgTime;
        m_congestionAwareThroughput = arrivedCount;
        m_congestionAwareMaxLoad = maxLoad;
    }
    
    // Update table
    if (m_shortestPathAvgTime > 0 || m_congestionAwareAvgTime > 0) {
        // Avg Trip Time
        QTableWidgetItem* item1 = m_comparisonTable->item(0, 1);
        if (item1) item1->setText(QString("%1").arg(m_shortestPathAvgTime, 0, 'f', 1));
        
        QTableWidgetItem* item2 = m_comparisonTable->item(0, 2);
        if (item2) {
            item2->setText(QString("%1").arg(m_congestionAwareAvgTime, 0, 'f', 1));
            if (m_congestionAwareAvgTime < m_shortestPathAvgTime && m_shortestPathAvgTime > 0) {
                item2->setBackground(QColor(16, 185, 129, 50)); // Green highlight
            }
        }
        
        // Throughput
        QTableWidgetItem* item3 = m_comparisonTable->item(1, 1);
        if (item3) item3->setText(QString::number(m_shortestPathThroughput));
        
        QTableWidgetItem* item4 = m_comparisonTable->item(1, 2);
        if (item4) {
            item4->setText(QString::number(m_congestionAwareThroughput));
            if (m_congestionAwareThroughput > m_shortestPathThroughput) {
                item4->setBackground(QColor(16, 185, 129, 50));
            }
        }
        
        // Max Load
        QTableWidgetItem* item5 = m_comparisonTable->item(2, 1);
        if (item5) item5->setText(QString::number(m_shortestPathMaxLoad));
        
        QTableWidgetItem* item6 = m_comparisonTable->item(2, 2);
        if (item6) {
            item6->setText(QString::number(m_congestionAwareMaxLoad));
            if (m_congestionAwareMaxLoad < m_shortestPathMaxLoad && m_shortestPathMaxLoad > 0) {
                item6->setBackground(QColor(16, 185, 129, 50));
            }
        }
        
        // Efficiency Score (calculated)
        double shortestEfficiency = m_shortestPathAvgTime > 0 ? 
            (100.0 / (1.0 + m_shortestPathAvgTime / 20.0)) : 0.0;
        double congestionEfficiency = m_congestionAwareAvgTime > 0 ?
            (100.0 / (1.0 + m_congestionAwareAvgTime / 20.0)) : 0.0;
        
        QTableWidgetItem* item7 = m_comparisonTable->item(3, 1);
        if (item7) item7->setText(QString("%1%").arg(shortestEfficiency, 0, 'f', 0));
        
        QTableWidgetItem* item8 = m_comparisonTable->item(3, 2);
        if (item8) {
            item8->setText(QString("%1%").arg(congestionEfficiency, 0, 'f', 0));
            if (congestionEfficiency > shortestEfficiency && shortestEfficiency > 0) {
                item8->setBackground(QColor(16, 185, 129, 50));
            }
        }
    }
}

QColor MetricsPanel::getGradientColor(double value, double min, double max) const {
    if (max == min) return QColor(16, 185, 129); // Green
    
    double normalized = (value - min) / (max - min);
    normalized = qBound(0.0, normalized, 1.0);
    
    if (normalized < 0.33) {
        // Green to Yellow
        double t = normalized / 0.33;
        return QColor(
            16 + (245 - 16) * t,
            185 + (158 - 185) * t,
            129 + (11 - 129) * t
        );
    } else if (normalized < 0.66) {
        // Yellow to Orange
        double t = (normalized - 0.33) / 0.33;
        return QColor(
            245 + (249 - 245) * t,
            158 + (115 - 158) * t,
            11 + (22 - 11) * t
        );
    } else {
        // Orange to Red
        double t = (normalized - 0.66) / 0.34;
        return QColor(
            249 + (239 - 249) * t,
            115 + (68 - 115) * t,
            22 + (68 - 22) * t
        );
    }
}

void MetricsPanel::animateStatCard(QLabel* label, double oldValue, double newValue) {
    if (m_counterAnimations.contains(label)) {
        QPropertyAnimation* anim = m_counterAnimations[label];
        anim->stop();
        delete anim;
    }
    
    QPropertyAnimation* anim = new QPropertyAnimation(label, "opacity", this);
    anim->setDuration(150);
    anim->setStartValue(0.5);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);
    anim->setTargetObject(effect);
    
    m_counterAnimations[label] = anim;
    anim->start();
}

void MetricsPanel::onExportImage() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Chart as Image",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/chart.png",
        "PNG Images (*.png);;JPEG Images (*.jpg)");
    
    if (!fileName.isEmpty()) {
        QPixmap pixmap = m_tripTimeChartView->grab();
        if (pixmap.save(fileName)) {
            QMessageBox::information(this, "Success", 
                QString("Chart saved to %1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Error", "Failed to save chart image");
        }
    }
}

void MetricsPanel::onCopyToClipboard() {
    QPixmap pixmap = m_tripTimeChartView->grab();
    QApplication::clipboard()->setPixmap(pixmap);
    QMessageBox::information(this, "Copied", "Chart copied to clipboard!");
}

void MetricsPanel::onShareChart() {
    QMessageBox::information(this, "Share Chart",
        "Chart sharing feature\n\n"
        "You can export the chart as an image and share it.\n"
        "Use 'Save Chart as Image' to export to PNG/JPEG.");
}
