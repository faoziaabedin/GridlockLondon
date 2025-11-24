#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class SimulationController;
class Metrics;

/**
 * Modern metrics panel displaying simulation statistics.
 */
class MetricsPanel : public QWidget {
    Q_OBJECT

public:
    explicit MetricsPanel(QWidget* parent = nullptr);
    
    void setSimulationController(SimulationController* controller);
    void updateMetrics();

private:
    void setupUI();
    void updateLabels();

    SimulationController* m_controller;
    
    // Labels
    QLabel* m_titleLabel;
    QLabel* m_tickLabel;
    QLabel* m_activeAgentsLabel;
    QLabel* m_arrivedAgentsLabel;
    QLabel* m_avgTravelTimeLabel;
    QLabel* m_maxEdgeLoadLabel;
};

