#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QTimer>
#include <QVector>
#include <QPaintEvent>

class SimulationController;
class Metrics;

/**
 * Modern metrics panel displaying simulation statistics with live charts.
 */
class MetricsPanel : public QWidget {
    Q_OBJECT

public:
    explicit MetricsPanel(QWidget* parent = nullptr);
    
    void setSimulationController(SimulationController* controller);
    void updateMetrics();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUI();
    void updateLabels();
    void drawChart(QPainter& painter, const QRect& rect, const QVector<double>& data, 
                   const QString& title, const QColor& color);
    
    SimulationController* m_controller;
    
    // Labels
    QLabel* m_titleLabel;
    QLabel* m_tickLabel;
    QLabel* m_activeAgentsLabel;
    QLabel* m_arrivedAgentsLabel;
    QLabel* m_avgTravelTimeLabel;
    QLabel* m_maxEdgeLoadLabel;
    
    // Chart data
    QVector<double> m_tripTimeHistory;
    QVector<double> m_throughputHistory;
    static constexpr int MAX_HISTORY = 100;
    
    // Comparison stats
    double m_shortestPathAvg;
    double m_congestionAwareAvg;
    bool m_hasComparison;
};
