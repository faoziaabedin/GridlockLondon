// code/ui/GridView.cpp
#include "GridView.h"
#include "../core/SimulationController.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"
#include "../core/Agent.h"
#include "../core/Preset.h"
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QScrollBar>
#include <QtMath>
#include <algorithm>
#include <cmath>

// ============================================================================
// NodeGraphicsItem Implementation - Traffic Intersection Style
// ============================================================================

NodeGraphicsItem::NodeGraphicsItem(NodeId id, const QPointF& position, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_nodeId(id), m_position(position), m_showLabel(false) {
    setPos(position);
    setAcceptHoverEvents(true);
    setZValue(10); // Nodes above edges
}

QRectF NodeGraphicsItem::boundingRect() const {
    qreal margin = 20;
    return QRectF(-margin, -margin, 2 * margin, 2 * margin);
}

void NodeGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Draw intersection circle (like a roundabout)
    qreal radius = 12.0;
    
    // Outer ring - asphalt color
    painter->setBrush(QColor(60, 60, 65));
    painter->setPen(QPen(QColor(80, 80, 85), 2));
    painter->drawEllipse(QRectF(-radius, -radius, 2 * radius, 2 * radius));
    
    // Inner circle - lighter center
    qreal innerRadius = 6.0;
    QRadialGradient centerGrad(0, 0, innerRadius);
    centerGrad.setColorAt(0, QColor(100, 100, 105));
    centerGrad.setColorAt(1, QColor(70, 70, 75));
    painter->setBrush(centerGrad);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QRectF(-innerRadius, -innerRadius, 2 * innerRadius, 2 * innerRadius));
    
    // Small center dot
    painter->setBrush(QColor(120, 120, 125));
    painter->drawEllipse(QRectF(-2, -2, 4, 4));
    
    // Label
    if (m_showLabel) {
        painter->setPen(QColor(200, 200, 200));
        QFont font("SF Pro Display", 9, QFont::Medium);
        painter->setFont(font);
        QRectF textRect(-15, -radius - 18, 30, 14);
        painter->drawText(textRect, Qt::AlignCenter, QString::number(m_nodeId));
    }
}

void NodeGraphicsItem::setShowLabel(bool show) {
    m_showLabel = show;
    update();
}

// ============================================================================
// EdgeGraphicsItem Implementation - Road Style
// ============================================================================

EdgeGraphicsItem::EdgeGraphicsItem(EdgeId id, NodeId from, NodeId to,
                                   const QPointF& fromPos, const QPointF& toPos,
                                   QGraphicsItem* parent)
    : QGraphicsObject(parent), m_edgeId(id), m_fromNode(from), m_toNode(to),
      m_fromPos(fromPos), m_toPos(toPos), m_currentColor(QColor(70, 70, 75)),
      m_targetColor(m_currentColor), m_occupancy(0), m_capacity(1), m_length(1.0), m_hovered(false) {
    setAcceptHoverEvents(true);
    setZValue(5); // Edges below nodes and agents
}

QRectF EdgeGraphicsItem::boundingRect() const {
    qreal margin = 15;
    QPointF topLeft(std::min(m_fromPos.x(), m_toPos.x()) - margin,
                    std::min(m_fromPos.y(), m_toPos.y()) - margin);
    QPointF bottomRight(std::max(m_fromPos.x(), m_toPos.x()) + margin,
                       std::max(m_fromPos.y(), m_toPos.y()) + margin);
    return QRectF(topLeft, bottomRight);
}

void EdgeGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Road width
    qreal roadWidth = m_hovered ? 14.0 : 12.0;
    
    // Calculate direction
    QLineF line(m_fromPos, m_toPos);
    
    // Draw road base (dark asphalt)
    QPen roadPen(QColor(50, 50, 55), roadWidth, Qt::SolidLine, Qt::RoundCap);
    painter->setPen(roadPen);
    painter->drawLine(m_fromPos, m_toPos);
    
    // Draw road surface with congestion color
    QPen surfacePen(m_currentColor, roadWidth - 4, Qt::SolidLine, Qt::RoundCap);
    painter->setPen(surfacePen);
    painter->drawLine(m_fromPos, m_toPos);
    
    // Draw center lane marking (dashed white/yellow line)
    if (line.length() > 30) {
        QPen lanePen(QColor(255, 200, 50, 150), 1.5, Qt::DashLine);
        lanePen.setDashPattern({4, 6});
        painter->setPen(lanePen);
        painter->drawLine(m_fromPos, m_toPos);
    }
    
    // Highlight on hover
    if (m_hovered) {
        QPen hoverPen(QColor(255, 255, 255, 80), roadWidth + 4, Qt::SolidLine, Qt::RoundCap);
        painter->setPen(hoverPen);
        painter->drawLine(m_fromPos, m_toPos);
    }
}

void EdgeGraphicsItem::setColor(const QColor& color) {
    if (m_currentColor != color) {
        m_currentColor = color;
        update();
    }
}

void EdgeGraphicsItem::setCongestion(int occupancy, int capacity, double length) {
    m_occupancy = occupancy;
    m_capacity = capacity;
    m_length = length;
    
    // Calculate congestion level and set color
    double ratio = capacity > 0 ? static_cast<double>(occupancy) / capacity : 0.0;
    
    if (ratio == 0.0) {
        m_targetColor = QColor(70, 80, 70); // Dark greenish gray - empty road
    } else if (ratio < 0.3) {
        m_targetColor = QColor(34, 139, 34); // Forest green - light traffic
    } else if (ratio < 0.6) {
        m_targetColor = QColor(218, 165, 32); // Goldenrod - moderate
    } else if (ratio < 0.85) {
        m_targetColor = QColor(255, 140, 0); // Dark orange - heavy
    } else {
        m_targetColor = QColor(220, 20, 60); // Crimson - gridlock
    }
}

QString EdgeGraphicsItem::tooltipText() const {
    double ratio = m_capacity > 0 ? static_cast<double>(m_occupancy) / m_capacity * 100.0 : 0.0;
    QString status;
    if (ratio == 0) status = "🟢 Clear";
    else if (ratio < 30) status = "🟢 Light Traffic";
    else if (ratio < 60) status = "🟡 Moderate";
    else if (ratio < 85) status = "🟠 Heavy";
    else status = "🔴 Gridlock!";
    
    return QString("🛣️ Road Segment %1\n"
                   "From: Intersection %2 → %3\n"
                   "Status: %4\n"
                   "Vehicles: %5 / %6 capacity")
            .arg(m_edgeId)
            .arg(m_fromNode)
            .arg(m_toNode)
            .arg(status)
            .arg(m_occupancy)
            .arg(m_capacity);
}

void EdgeGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    m_hovered = true;
    update();
    QToolTip::showText(event->screenPos(), tooltipText());
    QGraphicsItem::hoverEnterEvent(event);
}

void EdgeGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    m_hovered = false;
    update();
    QToolTip::hideText();
    QGraphicsItem::hoverLeaveEvent(event);
}

// ============================================================================
// AgentGraphicsItem Implementation - Car Style
// ============================================================================

AgentGraphicsItem::AgentGraphicsItem(Agent* agent, PolicyType policy, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_agent(agent), m_policy(policy), m_selected(false), m_showTrail(true) {
    setAcceptHoverEvents(true);
    setZValue(20); // Agents above everything
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF AgentGraphicsItem::boundingRect() const {
    qreal margin = 20;
    return QRectF(-margin, -margin, 2 * margin, 2 * margin);
}

void AgentGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Draw trail first (behind the car) if enabled
    if (m_showTrail && m_trail.size() > 1) {
        for (int i = 0; i < m_trail.size() - 1; ++i) {
            const auto& p1 = m_trail[i];
            const auto& p2 = m_trail[i + 1];
            
            QColor trailColor = p1.color;
            trailColor.setAlphaF(p1.opacity * 0.4);
            
            QPen trailPen(trailColor, 3, Qt::SolidLine, Qt::RoundCap);
            painter->setPen(trailPen);
            painter->drawLine(p1.pos, p2.pos);
        }
    }
    
    // Car colors based on policy
    QColor carBody, carAccent;
    if (m_policy == PolicyType::SHORTEST_PATH) {
        carBody = QColor(70, 130, 220);   // Blue car
        carAccent = QColor(100, 160, 255);
    } else {
        carBody = QColor(50, 180, 100);   // Green car
        carAccent = QColor(80, 220, 130);
    }
    
    // Selection glow
    if (m_selected) {
        QRadialGradient selGlow(0, 0, 18);
        selGlow.setColorAt(0, QColor(255, 255, 100, 150));
        selGlow.setColorAt(1, QColor(255, 255, 100, 0));
        painter->setBrush(selGlow);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QRectF(-18, -18, 36, 36));
    }
    
    // Draw car body (rounded rectangle, top-down view)
    qreal carLength = 14.0;
    qreal carWidth = 8.0;
    
    // Car shadow
    painter->setBrush(QColor(0, 0, 0, 60));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(QRectF(-carLength/2 + 2, -carWidth/2 + 2, carLength, carWidth), 3, 3);
    
    // Car body gradient
    QLinearGradient bodyGrad(-carLength/2, 0, carLength/2, 0);
    bodyGrad.setColorAt(0, carBody.darker(110));
    bodyGrad.setColorAt(0.5, carBody);
    bodyGrad.setColorAt(1, carBody.darker(110));
    
    painter->setBrush(bodyGrad);
    painter->setPen(QPen(carBody.darker(130), 1));
    painter->drawRoundedRect(QRectF(-carLength/2, -carWidth/2, carLength, carWidth), 3, 3);
    
    // Windshield (front window)
    painter->setBrush(QColor(150, 200, 255, 180));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(QRectF(carLength/2 - 5, -carWidth/2 + 1.5, 3, carWidth - 3), 1, 1);
    
    // Rear window
    painter->setBrush(QColor(100, 150, 200, 150));
    painter->drawRoundedRect(QRectF(-carLength/2 + 2, -carWidth/2 + 1.5, 2.5, carWidth - 3), 1, 1);
    
    // Headlights (front)
    painter->setBrush(QColor(255, 255, 200));
    painter->drawEllipse(QPointF(carLength/2 - 1, -carWidth/2 + 1.5), 1.2, 1.2);
    painter->drawEllipse(QPointF(carLength/2 - 1, carWidth/2 - 1.5), 1.2, 1.2);
    
    // Taillights (back)
    painter->setBrush(QColor(255, 50, 50));
    painter->drawEllipse(QPointF(-carLength/2 + 1.5, -carWidth/2 + 1.5), 1.0, 1.0);
    painter->drawEllipse(QPointF(-carLength/2 + 1.5, carWidth/2 - 1.5), 1.0, 1.0);
    
    // Direction indicator arrow
    painter->setBrush(carAccent);
    painter->setPen(QPen(Qt::white, 0.5));
    QPolygonF arrow;
    arrow << QPointF(carLength/2 + 4, 0)
          << QPointF(carLength/2 + 1, -2.5)
          << QPointF(carLength/2 + 1, 2.5);
    painter->drawPolygon(arrow);
}

void AgentGraphicsItem::setPosition(const QPointF& pos) {
    // Calculate rotation based on movement direction
    if (!m_position.isNull() && pos != m_position) {
        QLineF moveLine(m_position, pos);
        qreal angle = -moveLine.angle(); // Qt angles are counter-clockwise
        setRotation(angle);
    }
    
    m_position = pos;
    setPos(pos);
}

void AgentGraphicsItem::addTrailPoint(const QPointF& pos, const QColor& color) {
    TrailPoint point;
    // Convert to local coordinates for trail
    point.pos = mapFromScene(pos);
    point.opacity = 1.0;
    point.color = color;
    
    m_trail.append(point);
    if (m_trail.size() > MAX_TRAIL_POINTS) {
        m_trail.removeFirst();
    }
}

void AgentGraphicsItem::updateTrail() {
    // Fade out trail points
    for (auto& point : m_trail) {
        point.opacity = qMax(0.0, point.opacity - 0.08);
    }
    
    // Remove fully faded points
    m_trail.erase(std::remove_if(m_trail.begin(), m_trail.end(),
                                 [](const TrailPoint& p) { return p.opacity <= 0.0; }),
                 m_trail.end());
}

void AgentGraphicsItem::clearTrail() {
    m_trail.clear();
    update();
}

void AgentGraphicsItem::setSelected(bool selected) {
    m_selected = selected;
    update();
}

void AgentGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_agent) {
        setSelected(true);
        event->accept();
    }
    QGraphicsItem::mousePressEvent(event);
}

// ============================================================================
// GridView Implementation - City Map Style
// ============================================================================

GridView::GridView(QWidget* parent)
    : QGraphicsView(parent), m_controller(nullptr), m_scene(new QGraphicsScene(this)),
      m_zoomLevel(1.0), m_showGrid(true), m_showNodeLabels(false),
      m_showTrails(true), m_showHeatMap(false),
      m_panning(false), m_animationTimer(new QTimer(this)), m_selectedAgent(nullptr) {
    
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setBackgroundBrush(QColor(18, 21, 26)); // Match new theme
    setStyleSheet("background-color: #12151a; border: none;");
    
    // Animation timer for smooth updates
    m_animationTimer->setInterval(16); // ~60 FPS
    connect(m_animationTimer, &QTimer::timeout, this, &GridView::onAnimationFrame);
    m_animationTimer->start();
    
    setupScene();
}

GridView::~GridView() = default;

void GridView::setupScene() {
    m_scene->setSceneRect(-100, -100, 2000, 2000);
}

void GridView::setSimulationController(SimulationController* controller) {
    m_controller = controller;
    updateScene();
}

void GridView::updateScene() {
    if (!m_controller) {
        return;
    }
    
    City* city = m_controller->getCity();
    if (!city) {
        return;
    }
    
    // Stop any running animations
    for (auto* anim : m_agentAnimations.values()) {
        anim->stop();
        delete anim;
    }
    m_agentAnimations.clear();
    
    for (auto* anim : m_edgeColorAnimations.values()) {
        anim->stop();
        delete anim;
    }
    m_edgeColorAnimations.clear();
    
    // Clear existing items from scene
    for (auto* item : m_nodeItems.values()) {
        m_scene->removeItem(item);
        delete item;
    }
    m_nodeItems.clear();
    
    for (auto* item : m_edgeItems.values()) {
        m_scene->removeItem(item);
        delete item;
    }
    m_edgeItems.clear();
    
    for (auto* item : m_agentItems.values()) {
        m_scene->removeItem(item);
        delete item;
    }
    m_agentItems.clear();
    
    m_selectedAgent = nullptr;
    
    // Reset scene rect based on new city size
    int nodeCount = city->getNodeCount();
    if (nodeCount > 0) {
        // Estimate grid dimensions
        int maxRow = 0, maxCol = 0;
        for (int i = 0; i < nodeCount; ++i) {
            NodeId nodeId = city->getNodeIdByIndex(i);
            const Node& node = city->getNode(nodeId);
            maxRow = std::max(maxRow, node.getRow());
            maxCol = std::max(maxCol, node.getCol());
        }
        
        qreal sceneWidth = (maxCol + 2) * 100.0 + 200;
        qreal sceneHeight = (maxRow + 2) * 100.0 + 200;
        m_scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
    }
    
    // Create graphics items in correct order
    createEdgeItems();   // Edges first (below)
    createNodeItems();   // Then nodes
    createAgentItems();  // Agents on top
    
    // Update edge colors
    updateEdgeColors();
    
    // Force viewport update
    viewport()->update();
    
    // Fit to window
    fitToWindow();
}

void GridView::createNodeItems() {
    if (!m_controller || !m_controller->getCity()) return;
    
    City* city = m_controller->getCity();
    
    // Calculate spacing for a nice grid layout
    const qreal SPACING = 100.0;
    qreal startX = 150;
    qreal startY = 150;
    
    int nodeCount = city->getNodeCount();
    for (int i = 0; i < nodeCount; ++i) {
        NodeId nodeId = city->getNodeIdByIndex(i);
        const Node& node = city->getNode(nodeId);
        QPointF pos(startX + node.getCol() * SPACING, startY + node.getRow() * SPACING);
        
        NodeGraphicsItem* item = new NodeGraphicsItem(nodeId, pos);
        item->setShowLabel(m_showNodeLabels);
        m_scene->addItem(item);
        m_nodeItems[nodeId] = item;
    }
}

void GridView::createEdgeItems() {
    if (!m_controller || !m_controller->getCity()) return;
    
    City* city = m_controller->getCity();
    
    // We need node positions first, so create a temporary map
    const qreal SPACING = 100.0;
    qreal startX = 150;
    qreal startY = 150;
    
    QMap<NodeId, QPointF> nodePositions;
    int nodeCount = city->getNodeCount();
    for (int i = 0; i < nodeCount; ++i) {
        NodeId nodeId = city->getNodeIdByIndex(i);
        const Node& node = city->getNode(nodeId);
        nodePositions[nodeId] = QPointF(startX + node.getCol() * SPACING, 
                                        startY + node.getRow() * SPACING);
    }
    
    int edgeCount = city->getEdgeCount();
    for (int i = 0; i < edgeCount; ++i) {
        EdgeId edgeId = city->getEdgeIdByIndex(i);
        const Edge& edge = city->getEdge(edgeId);
        
        QPointF fromPos = nodePositions.value(edge.getFrom(), QPointF());
        QPointF toPos = nodePositions.value(edge.getTo(), QPointF());
        
        if (fromPos.isNull() || toPos.isNull()) continue;
        
        EdgeGraphicsItem* item = new EdgeGraphicsItem(edgeId, edge.getFrom(), edge.getTo(), fromPos, toPos);
        m_scene->addItem(item);
        m_edgeItems[edgeId] = item;
    }
}

void GridView::createAgentItems() {
    if (!m_controller) return;
    
    auto& agents = m_controller->getAgents();
    PolicyType policy = m_controller->getPolicy();
    
    for (Agent* agent : agents) {
        if (!agent) continue;
        
        QPointF pos = nodePosition(agent->getCurrentNode());
        AgentGraphicsItem* item = new AgentGraphicsItem(agent, policy);
        item->setPosition(pos);
        item->setVisible(!agent->hasArrived());
        item->setShowTrail(m_showTrails);
        m_scene->addItem(item);
        m_agentItems[agent] = item;
    }
}

QPointF GridView::nodePosition(NodeId nodeId) const {
    if (m_nodeItems.contains(nodeId)) {
        return m_nodeItems[nodeId]->pos();
    }
    return QPointF(0, 0);
}

QPointF GridView::interpolatePosition(NodeId from, NodeId to, qreal progress) const {
    QPointF fromPos = nodePosition(from);
    QPointF toPos = nodePosition(to);
    return fromPos + (toPos - fromPos) * progress;
}

void GridView::updateEdgeColors() {
    if (!m_controller || !m_controller->getCity()) return;
    
    City* city = m_controller->getCity();
    PolicyType currentPolicy = m_controller->getPolicy();
    
    for (auto it = m_edgeItems.begin(); it != m_edgeItems.end(); ++it) {
        EdgeId edgeId = it.key();
        EdgeGraphicsItem* item = it.value();
        
        const Edge& edge = city->getEdge(edgeId);
        int occupancy = city->occupancy(edgeId);
        int capacity = edge.getCapacity();
        double length = edge.getLength();
        
        item->setCongestion(occupancy, capacity, length);
        
        // Get target color based on policy and occupancy
        QColor targetColor;
        if (m_showHeatMap && currentPolicy == PolicyType::CONGESTION_AWARE) {
            // For congestion-aware, show "perceived cost" - 
            // roads with ANY traffic are penalized more
            targetColor = getCongestionAwareColor(occupancy, capacity);
        } else {
            targetColor = getCongestionColor(occupancy, capacity);
        }
        item->setColor(targetColor);
    }
}

QColor GridView::getCongestionColor(int occupancy, int capacity) const {
    double ratio = capacity > 0 ? static_cast<double>(occupancy) / capacity : 0.0;
    
    if (m_showHeatMap) {
        // Vivid heat map colors - lower thresholds to see more variation
        if (occupancy == 0) {
            return QColor(50, 160, 50); // Green - empty road
        } else if (ratio <= 0.33) {
            return QColor(120, 200, 80); // Light green - 1 car (light traffic)
        } else if (ratio <= 0.66) {
            return QColor(255, 200, 0); // Yellow/Gold - 2 cars (moderate)
        } else if (ratio < 1.0) {
            return QColor(255, 120, 0); // Orange - almost full
        } else {
            return QColor(255, 50, 50); // Red - at capacity (gridlock)
        }
    } else {
        // Subtle default colors when heat map is off
        if (occupancy == 0) {
            return QColor(80, 100, 80); // Grayish green - empty
        } else if (ratio <= 0.33) {
            return QColor(60, 140, 60); // Forest green - light
        } else if (ratio <= 0.66) {
            return QColor(180, 150, 50); // Olive/gold - moderate
        } else if (ratio < 1.0) {
            return QColor(200, 100, 50); // Orange - heavy
        } else {
            return QColor(180, 60, 60); // Red - gridlock
        }
    }
}

QColor GridView::getCongestionAwareColor(int occupancy, int capacity) const {
    // Congestion-aware view: roads with ANY traffic show as more "expensive"
    // This visualizes how the congestion-aware algorithm "sees" the network
    double ratio = capacity > 0 ? static_cast<double>(occupancy) / capacity : 0.0;
    
    if (occupancy == 0) {
        return QColor(50, 180, 80); // Bright green - preferred route (empty)
    } else if (occupancy == 1) {
        return QColor(200, 200, 50); // Yellow - slightly penalized
    } else if (ratio <= 0.5) {
        return QColor(255, 150, 0); // Orange - avoid if possible
    } else if (ratio < 1.0) {
        return QColor(255, 80, 0); // Dark orange - strongly avoid
    } else {
        return QColor(220, 30, 30); // Red - blocked/full
    }
}

void GridView::updateAgentPositions() {
    if (!m_controller) return;
    
    auto& agents = m_controller->getAgents();
    
    // Build set of valid agents
    QSet<Agent*> validAgents;
    for (Agent* agent : agents) {
        if (agent) validAgents.insert(agent);
    }
    
    // Remove invalid agent items
    QList<Agent*> agentsToRemove;
    for (auto it = m_agentItems.begin(); it != m_agentItems.end(); ++it) {
        if (!validAgents.contains(it.key())) {
            agentsToRemove.append(it.key());
        }
    }
    for (Agent* agent : agentsToRemove) {
        AgentGraphicsItem* item = m_agentItems[agent];
        m_scene->removeItem(item);
        delete item;
        m_agentItems.remove(agent);
        if (m_selectedAgent && m_selectedAgent->agent() == agent) {
            m_selectedAgent = nullptr;
        }
    }
    
    // Update agent positions
    for (Agent* agent : agents) {
        if (!agent || !m_agentItems.contains(agent)) continue;
        
        AgentGraphicsItem* item = m_agentItems[agent];
        
        // Hide arrived agents
        if (agent->hasArrived()) {
            item->setVisible(false);
            continue;
        }
        
        item->setVisible(true);
        
        QPointF targetPos;
        if (agent->getCurrentEdge().has_value()) {
            EdgeId edgeId = agent->getCurrentEdge().value();
            const Edge& edge = m_controller->getCity()->getEdge(edgeId);
            targetPos = interpolatePosition(edge.getFrom(), edge.getTo(), 0.5);
        } else {
            targetPos = nodePosition(agent->getCurrentNode());
        }
        
        // Move agent and update trail
        item->setPosition(targetPos);
        
        QColor trailColor = (m_controller->getPolicy() == PolicyType::CONGESTION_AWARE ? 
                            QColor(50, 180, 100) : QColor(70, 130, 220));
        item->addTrailPoint(targetPos, trailColor);
    }
}

void GridView::animateStep() {
    updateEdgeColors();
    updateAgentPositions();
    
    for (auto* item : m_agentItems.values()) {
        item->updateTrail();
    }
}

void GridView::onAnimationFrame() {
    for (auto* item : m_agentItems.values()) {
        item->updateTrail();
    }
    viewport()->update();
}

void GridView::fitToWindow() {
    if (m_scene->items().isEmpty()) return;
    
    QRectF itemsRect = m_scene->itemsBoundingRect();
    if (itemsRect.isEmpty()) return;
    
    itemsRect.adjust(-80, -80, 80, 80);
    fitInView(itemsRect, Qt::KeepAspectRatio);
    m_zoomLevel = transform().m11();
}

void GridView::setShowGrid(bool show) {
    m_showGrid = show;
    viewport()->update();
}

void GridView::setShowNodeLabels(bool show) {
    m_showNodeLabels = show;
    for (auto* item : m_nodeItems.values()) {
        item->setShowLabel(show);
    }
}

void GridView::setShowTrails(bool show) {
    m_showTrails = show;
    for (auto* item : m_agentItems.values()) {
        item->setShowTrail(show);
        if (!show) {
            item->clearTrail();
        }
    }
    viewport()->update();
}

void GridView::setShowHeatMap(bool show) {
    m_showHeatMap = show;
    // Update edge colors to show heat map
    updateEdgeColors();
    viewport()->update();
}

void GridView::setZoomLevel(qreal level) {
    level = qBound(0.2, level, 4.0);
    if (qAbs(level - m_zoomLevel) < 0.01) return;
    
    m_zoomLevel = level;
    QTransform t;
    t.scale(level, level);
    setTransform(t);
}

void GridView::wheelEvent(QWheelEvent* event) {
    const qreal scaleFactor = 1.15;
    qreal zoom = m_zoomLevel;
    
    if (event->angleDelta().y() > 0) {
        zoom *= scaleFactor;
    } else {
        zoom /= scaleFactor;
    }
    
    setZoomLevel(zoom);
    event->accept();
}

void GridView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton || 
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)) {
        m_panning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());
        AgentGraphicsItem* agentItem = qgraphicsitem_cast<AgentGraphicsItem*>(item);
        
        if (m_selectedAgent && m_selectedAgent != agentItem) {
            m_selectedAgent->setSelected(false);
        }
        
        if (agentItem) {
            Agent* agent = agentItem->agent();
            if (agent && m_controller) {
                bool isValid = false;
                auto& agents = m_controller->getAgents();
                for (Agent* a : agents) {
                    if (a == agent) {
                        isValid = true;
                        break;
                    }
                }
                if (isValid) {
                    m_selectedAgent = agentItem;
                    agentItem->setSelected(true);
                    emit agentSelected(agent);
                } else {
                    m_selectedAgent = nullptr;
                    emit agentSelected(nullptr);
                }
            }
        } else {
            m_selectedAgent = nullptr;
            emit agentSelected(nullptr);
        }
    }
    
    QGraphicsView::mousePressEvent(event);
}

void GridView::mouseMoveEvent(QMouseEvent* event) {
    if (m_panning) {
        QPoint delta = event->pos() - m_lastPanPoint;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastPanPoint = event->pos();
        event->accept();
        return;
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void GridView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void GridView::keyPressEvent(QKeyEvent* event) {
    const int step = 30;
    switch (event->key()) {
        case Qt::Key_Left:
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - step);
            break;
        case Qt::Key_Right:
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + step);
            break;
        case Qt::Key_Up:
            verticalScrollBar()->setValue(verticalScrollBar()->value() - step);
            break;
        case Qt::Key_Down:
            verticalScrollBar()->setValue(verticalScrollBar()->value() + step);
            break;
        default:
            QGraphicsView::keyPressEvent(event);
    }
}

void GridView::drawBackground(QPainter* painter, const QRectF& rect) {
    // Soft grass green background
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(76, 132, 82));    // Soft sage green
    gradient.setColorAt(0.5, QColor(88, 145, 92));  // Light grass
    gradient.setColorAt(1, QColor(65, 117, 70));    // Deeper green
    painter->fillRect(rect, gradient);
    
    // Draw grass texture pattern
    if (m_showGrid) {
        const qreal blockSize = 80.0;
        
        qreal left = qFloor(rect.left() / blockSize) * blockSize;
        qreal top = qFloor(rect.top() / blockSize) * blockSize;
        
        painter->setPen(Qt::NoPen);
        painter->setRenderHint(QPainter::Antialiasing, true);
        
        // Draw park blocks (grass patches)
        for (qreal x = left; x < rect.right(); x += blockSize) {
            for (qreal y = top; y < rect.bottom(); y += blockSize) {
                int shade = (static_cast<int>(x + y) / static_cast<int>(blockSize)) % 2;
                if (shade == 0) {
                    painter->setBrush(QColor(95, 155, 100, 40));
                } else {
                    painter->setBrush(QColor(75, 135, 80, 30));
                }
                QRectF block(x + 8, y + 8, blockSize - 16, blockSize - 16);
                painter->drawRoundedRect(block, 6, 6);
            }
        }
        
        // Draw realistic trees scattered around
        const qreal treeSpacing = 140.0;
        qreal treeLeft = qFloor(rect.left() / treeSpacing) * treeSpacing;
        qreal treeTop = qFloor(rect.top() / treeSpacing) * treeSpacing;
        
        for (qreal x = treeLeft; x < rect.right(); x += treeSpacing) {
            for (qreal y = treeTop; y < rect.bottom(); y += treeSpacing) {
                // Pseudo-random offset for natural placement
                int seed = static_cast<int>(x * 7 + y * 13);
                qreal offsetX = (seed % 50) - 25;
                qreal offsetY = ((seed * 3) % 50) - 25;
                qreal treeX = x + offsetX;
                qreal treeY = y + offsetY;
                
                // Tree size variation
                qreal scale = 0.8 + ((seed % 5) * 0.1);
                
                // Shadow under tree
                painter->setBrush(QColor(40, 70, 45, 60));
                painter->drawEllipse(QPointF(treeX + 2, treeY + 12 * scale), 10 * scale, 5 * scale);
                
                // Tree trunk
                painter->setBrush(QColor(92, 64, 51));  // Warm brown
                painter->drawRoundedRect(QRectF(treeX - 2 * scale, treeY, 4 * scale, 12 * scale), 2, 2);
                
                // Trunk detail
                painter->setPen(QPen(QColor(72, 50, 40), 1));
                painter->drawLine(QPointF(treeX, treeY + 2), QPointF(treeX, treeY + 10 * scale));
                painter->setPen(Qt::NoPen);
                
                // Tree canopy - layered for depth
                // Back layer (shadow)
                painter->setBrush(QColor(45, 90, 50));
                painter->drawEllipse(QPointF(treeX + 1, treeY - 4 * scale), 12 * scale, 10 * scale);
                
                // Middle layer
                painter->setBrush(QColor(60, 115, 65));
                painter->drawEllipse(QPointF(treeX - 2, treeY - 6 * scale), 11 * scale, 9 * scale);
                
                // Front highlight
                painter->setBrush(QColor(80, 140, 85));
                painter->drawEllipse(QPointF(treeX - 3, treeY - 8 * scale), 8 * scale, 7 * scale);
                
                // Top highlight
                painter->setBrush(QColor(100, 160, 105, 180));
                painter->drawEllipse(QPointF(treeX - 4, treeY - 10 * scale), 5 * scale, 4 * scale);
            }
        }
        
        // Draw subtle grid lines
        painter->setPen(QPen(QColor(50, 80, 55, 25), 1, Qt::DotLine));
        
        for (qreal x = left; x < rect.right(); x += blockSize) {
            painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
        }
        
        for (qreal y = top; y < rect.bottom(); y += blockSize) {
            painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
        }
    }
}

void GridView::updateAgentTrail(AgentGraphicsItem* agentItem, Agent* agent) {
    Q_UNUSED(agentItem);
    Q_UNUSED(agent);
}

void GridView::clearAgentTrails() {
    for (auto* item : m_agentItems.values()) {
        // Trails are cleared in AgentGraphicsItem
    }
}
