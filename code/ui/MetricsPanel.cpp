#include "MetricsPanel.h"
#include "../core/SimulationController.h"
#include "../core/Metrics.h"
#include "../core/Agent.h"
#include <QVBoxLayout>
#include <QLabel>

MetricsPanel::MetricsPanel(QWidget* parent)
    : QWidget(parent), m_controller(nullptr) {
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
    
    m_titleLabel = new QLabel("Metrics", this);
    m_titleLabel->setObjectName("sectionTitle");
    layout->addWidget(m_titleLabel);
    
    m_tickLabel = new QLabel("Tick: 0", this);
    m_tickLabel->setObjectName("metricLabel");
    layout->addWidget(m_tickLabel);
    
    m_activeAgentsLabel = new QLabel("Active Agents: 0", this);
    m_activeAgentsLabel->setObjectName("metricLabel");
    layout->addWidget(m_activeAgentsLabel);
    
    m_arrivedAgentsLabel = new QLabel("Arrived Agents: 0", this);
    m_arrivedAgentsLabel->setObjectName("metricLabel");
    layout->addWidget(m_arrivedAgentsLabel);
    
    m_avgTravelTimeLabel = new QLabel("Avg Travel Time: 0", this);
    m_avgTravelTimeLabel->setObjectName("metricLabel");
    layout->addWidget(m_avgTravelTimeLabel);
    
    m_maxEdgeLoadLabel = new QLabel("Max Edge Load: 0", this);
    m_maxEdgeLoadLabel->setObjectName("metricLabel");
    layout->addWidget(m_maxEdgeLoadLabel);
    
    layout->addStretch();
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
    
    m_tickLabel->setText(QString("Tick: %1").arg(metrics->getCurrentTick()));
    m_activeAgentsLabel->setText(QString("Active Agents: %1").arg(activeCount));
    m_arrivedAgentsLabel->setText(QString("Arrived Agents: %1").arg(arrivedCount));
    m_avgTravelTimeLabel->setText(QString("Avg Travel Time: %1").arg(avgTravelTime, 0, 'f', 1));
    m_maxEdgeLoadLabel->setText(QString("Max Edge Load: %1").arg(metrics->getMaxEdgeLoad()));
}

void MetricsPanel::updateLabels() {
    m_tickLabel->setText("Tick: 0");
    m_activeAgentsLabel->setText("Active Agents: 0");
    m_arrivedAgentsLabel->setText("Arrived Agents: 0");
    m_avgTravelTimeLabel->setText("Avg Travel Time: 0");
    m_maxEdgeLoadLabel->setText("Max Edge Load: 0");
}

