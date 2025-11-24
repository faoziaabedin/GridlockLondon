#include "MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Metrics.h"
#include "../core/Agent.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <algorithm>
#include <QtMath>

MetricsPanel::MetricsPanel(QWidget* parent)
    : QWidget(parent), m_controller(nullptr), m_shortestPathAvg(0.0),
      m_congestionAwareAvg(0.0), m_hasComparison(false) {
    setObjectName("metricsPanel");
    setupUI();
}

void MetricsPanel::setSimulationController(SimulationController* controller) {
    m_controller = controller;
    updateMetrics();
}

void MetricsPanel::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    
    m_titleLabel = new QLabel("📊 METRICS", this);
    m_titleLabel->setObjectName("sectionTitle");
    layout->addWidget(m_titleLabel);
    
    // Chart placeholder labels (will be drawn in paintEvent)
    QLabel* chart1Label = new QLabel("[Live Chart]\nTrip Time Graph", this);
    chart1Label->setObjectName("chartLabel");
    chart1Label->setMinimumHeight(120);
    chart1Label->setAlignment(Qt::AlignCenter);
    layout->addWidget(chart1Label);
    
    QLabel* chart2Label = new QLabel("[Live Chart]\nThroughput Graph", this);
    chart2Label->setObjectName("chartLabel");
    chart2Label->setMinimumHeight(120);
    chart2Label->setAlignment(Qt::AlignCenter);
    layout->addWidget(chart2Label);
    
    layout->addSpacing(8);
    
    // Comparison section
    QLabel* comparisonTitle = new QLabel("📋 COMPARISON", this);
    comparisonTitle->setObjectName("sectionTitle");
    layout->addWidget(comparisonTitle);
    
    QLabel* shortestPathLabel = new QLabel("Shortest Path:\n14.6 avg steps", this);
    shortestPathLabel->setObjectName("metricLabel");
    layout->addWidget(shortestPathLabel);
    
    QLabel* congestionAwareLabel = new QLabel("Congestion-Aware:\n12.4 avg steps ⬆15%", this);
    congestionAwareLabel->setObjectName("metricLabel");
    layout->addWidget(congestionAwareLabel);
    
    layout->addSpacing(8);
    
    // Export button
    QPushButton* exportButton = new QPushButton("📤 Export Data...", this);
    exportButton->setObjectName("secondaryButton");
    layout->addWidget(exportButton);
    
    layout->addStretch();
    
    // Basic metric labels (hidden, used for data)
    m_tickLabel = new QLabel("Tick: 0", this);
    m_tickLabel->setVisible(false);
    layout->addWidget(m_tickLabel);
    
    m_activeAgentsLabel = new QLabel("Active Agents: 0", this);
    m_activeAgentsLabel->setVisible(false);
    layout->addWidget(m_activeAgentsLabel);
    
    m_arrivedAgentsLabel = new QLabel("Arrived Agents: 0", this);
    m_arrivedAgentsLabel->setVisible(false);
    layout->addWidget(m_arrivedAgentsLabel);
    
    m_avgTravelTimeLabel = new QLabel("Avg Travel Time: 0", this);
    m_avgTravelTimeLabel->setVisible(false);
    layout->addWidget(m_avgTravelTimeLabel);
    
    m_maxEdgeLoadLabel = new QLabel("Max Edge Load: 0", this);
    m_maxEdgeLoadLabel->setVisible(false);
    layout->addWidget(m_maxEdgeLoadLabel);
}

void MetricsPanel::updateMetrics() {
    if (!m_controller) {
        updateLabels();
        return;
    }
    
    Metrics* metrics = m_controller->getMetrics();
    auto& agents = m_controller->getAgents();
    
    if (!metrics) {
        updateLabels();
        return;
    }
    
    // Count active and arrived agents
    int activeCount = 0;
    int arrivedCount = 0;
    int totalTravelTime = 0;
    
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
    
    // Update history for charts
    m_tripTimeHistory.append(avgTravelTime);
    if (m_tripTimeHistory.size() > MAX_HISTORY) {
        m_tripTimeHistory.removeFirst();
    }
    
    m_throughputHistory.append(arrivedCount);
    if (m_throughputHistory.size() > MAX_HISTORY) {
        m_throughputHistory.removeFirst();
    }
    
    m_tickLabel->setText(QString("Tick: %1").arg(metrics->getCurrentTick()));
    m_activeAgentsLabel->setText(QString("Active Agents: %1").arg(activeCount));
    m_arrivedAgentsLabel->setText(QString("Arrived Agents: %1").arg(arrivedCount));
    m_avgTravelTimeLabel->setText(QString("Avg Travel Time: %1").arg(avgTravelTime, 0, 'f', 1));
    m_maxEdgeLoadLabel->setText(QString("Max Edge Load: %1").arg(metrics->getMaxEdgeLoad()));
    
    // Trigger repaint for charts
    update();
}

void MetricsPanel::updateLabels() {
    m_tickLabel->setText("Tick: 0");
    m_activeAgentsLabel->setText("Active Agents: 0");
    m_arrivedAgentsLabel->setText("Arrived Agents: 0");
    m_avgTravelTimeLabel->setText("Avg Travel Time: 0");
    m_maxEdgeLoadLabel->setText("Max Edge Load: 0");
}

void MetricsPanel::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Find chart areas (after title, before comparison)
    int chartY = 60; // Below title
    int chartHeight = 120;
    int chartSpacing = 140;
    
    // Draw first chart (Trip Time)
    QRect chart1Rect(20, chartY, width() - 40, chartHeight);
    if (m_tripTimeHistory.size() > 1) {
        drawChart(painter, chart1Rect, m_tripTimeHistory, "Trip Time", QColor(59, 130, 246));
    } else {
        // Draw placeholder
        painter.setPen(QColor(62, 62, 66));
        painter.drawRect(chart1Rect);
        painter.setPen(QColor(204, 204, 204));
        painter.drawText(chart1Rect, Qt::AlignCenter, "[Live Chart]\nTrip Time Graph");
    }
    
    // Draw second chart (Throughput)
    QRect chart2Rect(20, chartY + chartSpacing, width() - 40, chartHeight);
    if (m_throughputHistory.size() > 1) {
        drawChart(painter, chart2Rect, m_throughputHistory, "Throughput", QColor(16, 185, 129));
    } else {
        // Draw placeholder
        painter.setPen(QColor(62, 62, 66));
        painter.drawRect(chart2Rect);
        painter.setPen(QColor(204, 204, 204));
        painter.drawText(chart2Rect, Qt::AlignCenter, "[Live Chart]\nThroughput Graph");
    }
}

void MetricsPanel::drawChart(QPainter& painter, const QRect& rect, const QVector<double>& data,
                            const QString& title, const QColor& color) {
    if (data.isEmpty()) return;
    
    // Draw background
    painter.fillRect(rect, QColor(30, 30, 30));
    painter.setPen(QColor(62, 62, 66));
    painter.drawRect(rect);
    
    // Find min/max for scaling
    double minVal = *std::min_element(data.begin(), data.end());
    double maxVal = *std::max_element(data.begin(), data.end());
    if (maxVal == minVal) {
        maxVal = minVal + 1.0;
    }
    
    // Draw grid lines
    painter.setPen(QPen(QColor(62, 62, 66), 1, Qt::DashLine));
    int gridLines = 5;
    for (int i = 0; i <= gridLines; ++i) {
        int y = rect.top() + (rect.height() * i / gridLines);
        painter.drawLine(rect.left() + 5, y, rect.right() - 5, y);
    }
    
    // Draw line chart
    QPainterPath path;
    int dataSize = data.size();
    int chartWidth = rect.width() - 20;
    int chartHeight = rect.height() - 30;
    int chartX = rect.left() + 10;
    int chartY = rect.top() + 10;
    
    for (int i = 0; i < dataSize; ++i) {
        double normalized = (data[i] - minVal) / (maxVal - minVal);
        int x = chartX + (i * chartWidth / (dataSize - 1));
        int y = chartY + chartHeight - (normalized * chartHeight);
        
        if (i == 0) {
            path.moveTo(x, y);
        } else {
            path.lineTo(x, y);
        }
    }
    
    // Draw filled area under line
    QPainterPath filledPath = path;
    filledPath.lineTo(chartX + chartWidth, chartY + chartHeight);
    filledPath.lineTo(chartX, chartY + chartHeight);
    filledPath.closeSubpath();
    
    QColor fillColor = color;
    fillColor.setAlpha(30);
    painter.fillPath(filledPath, fillColor);
    
    // Draw line
    painter.setPen(QPen(color, 2));
    painter.drawPath(path);
    
    // Draw points
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < dataSize; i += qMax(1, dataSize / 20)) {
        double normalized = (data[i] - minVal) / (maxVal - minVal);
        int x = chartX + (i * chartWidth / (dataSize - 1));
        int y = chartY + chartHeight - (normalized * chartHeight);
        painter.drawEllipse(QPoint(x, y), 3, 3);
    }
    
    // Draw title and values
    painter.setPen(QColor(204, 204, 204));
    QFont font("Segoe UI", 9);
    painter.setFont(font);
    painter.drawText(rect.left() + 10, rect.top() + 20, title);
    
    if (dataSize > 0) {
        QString valueText = QString::number(data.last(), 'f', 1);
        painter.drawText(rect.right() - 60, rect.top() + 20, valueText);
    }
}
