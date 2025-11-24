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

// ============================================================================
// NodeGraphicsItem Implementation
// ============================================================================

NodeGraphicsItem::NodeGraphicsItem(NodeId id, const QPointF& position, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_nodeId(id), m_position(position), m_showLabel(false) {
    setPos(position);
    setAcceptHoverEvents(true);
    setZValue(10); // Nodes above edges
}

QRectF NodeGraphicsItem::boundingRect() const {
    qreal margin = GLOW_RADIUS;
    return QRectF(-margin, -margin, 2 * margin, 2 * margin);
}

void NodeGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Glow effect
    QRadialGradient glowGradient(0, 0, GLOW_RADIUS);
    glowGradient.setColorAt(0, QColor(100, 150, 255, 80));
    glowGradient.setColorAt(0.5, QColor(100, 150, 255, 40));
    glowGradient.setColorAt(1, QColor(100, 150, 255, 0));
    
    painter->setBrush(glowGradient);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QRectF(-GLOW_RADIUS, -GLOW_RADIUS, 2 * GLOW_RADIUS, 2 * GLOW_RADIUS));
    
    // Node circle
    QRadialGradient nodeGradient(0, 0, NODE_RADIUS);
    nodeGradient.setColorAt(0, QColor(150, 200, 255));
    nodeGradient.setColorAt(1, QColor(100, 150, 255));
    
    painter->setBrush(nodeGradient);
    painter->setPen(QPen(QColor(200, 220, 255), 1.5));
    painter->drawEllipse(QRectF(-NODE_RADIUS, -NODE_RADIUS, 2 * NODE_RADIUS, 2 * NODE_RADIUS));
    
    // Label
    if (m_showLabel) {
        painter->setPen(QColor(200, 220, 255));
        QFont font("Inter", 8, QFont::Medium);
        painter->setFont(font);
        QRectF textRect(-20, -NODE_RADIUS - 15, 40, 12);
        painter->drawText(textRect, Qt::AlignCenter, QString::number(m_nodeId));
    }
}

void NodeGraphicsItem::setShowLabel(bool show) {
    m_showLabel = show;
    update();
}

// ============================================================================
// EdgeGraphicsItem Implementation
// ============================================================================

EdgeGraphicsItem::EdgeGraphicsItem(EdgeId id, NodeId from, NodeId to,
                                   const QPointF& fromPos, const QPointF& toPos,
                                   QGraphicsItem* parent)
    : QGraphicsObject(parent), m_edgeId(id), m_fromNode(from), m_toNode(to),
      m_fromPos(fromPos), m_toPos(toPos), m_currentColor(QColor(59, 130, 246)), // Cool blue
      m_targetColor(m_currentColor), m_occupancy(0), m_capacity(1), m_length(1.0), m_hovered(false) {
    setAcceptHoverEvents(true);
    setZValue(5); // Edges below nodes and agents
}

QRectF EdgeGraphicsItem::boundingRect() const {
    QPointF topLeft(std::min(m_fromPos.x(), m_toPos.x()) - EDGE_WIDTH,
                    std::min(m_fromPos.y(), m_toPos.y()) - EDGE_WIDTH);
    QPointF bottomRight(std::max(m_fromPos.x(), m_toPos.x()) + EDGE_WIDTH,
                       std::max(m_fromPos.y(), m_toPos.y()) + EDGE_WIDTH);
    return QRectF(topLeft, bottomRight);
}

void EdgeGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    QPen pen;
    if (m_hovered) {
        pen = QPen(QColor(255, 255, 255, 200), EDGE_WIDTH + 2);
    } else {
        pen = QPen(m_currentColor, EDGE_WIDTH);
    }
    
    painter->setPen(pen);
    painter->drawLine(m_fromPos, m_toPos);
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
    
    // Calculate congestion level
    double ratio = capacity > 0 ? static_cast<double>(occupancy) / capacity : 0.0;
    
    if (ratio == 0.0) {
        m_targetColor = QColor(59, 130, 246); // Cool blue - Empty
    } else if (ratio < 0.3) {
        m_targetColor = QColor(6, 182, 212); // Cyan - Light traffic
    } else if (ratio < 0.7) {
        m_targetColor = QColor(245, 158, 11); // Yellow - Medium
    } else if (ratio < 1.0) {
        m_targetColor = QColor(249, 115, 22); // Orange - Heavy
    } else {
        m_targetColor = QColor(239, 68, 68); // Red - Gridlock
    }
}

QString EdgeGraphicsItem::tooltipText() const {
    double ratio = m_capacity > 0 ? static_cast<double>(m_occupancy) / m_capacity * 100.0 : 0.0;
    return QString("Edge %1: Node %2 → Node %3\n"
                   "Capacity: %4 vehicles\n"
                   "Current: %5 vehicles (%6%)\n"
                   "Length: %7 units")
            .arg(m_edgeId)
            .arg(m_fromNode)
            .arg(m_toNode)
            .arg(m_capacity)
            .arg(m_occupancy)
            .arg(ratio, 0, 'f', 1)
            .arg(m_length, 0, 'f', 1);
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
// AgentGraphicsItem Implementation
// ============================================================================

AgentGraphicsItem::AgentGraphicsItem(Agent* agent, PolicyType policy, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_agent(agent), m_policy(policy), m_selected(false) {
    setAcceptHoverEvents(true);
    setZValue(20); // Agents above everything
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF AgentGraphicsItem::boundingRect() const {
    qreal margin = AGENT_RADIUS + 2;
    return QRectF(-margin, -margin, 2 * margin, 2 * margin);
}

void AgentGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // Draw trail
    if (m_trail.size() > 1) {
        QPen trailPen;
        for (int i = 0; i < m_trail.size() - 1; ++i) {
            const auto& p1 = m_trail[i];
            const auto& p2 = m_trail[i + 1];
            
            QColor trailColor = p1.color;
            trailColor.setAlphaF(p1.opacity);
            trailPen.setColor(trailColor);
            trailPen.setWidth(2);
            painter->setPen(trailPen);
            painter->drawLine(p1.pos, p2.pos);
        }
    }
    
    // Agent color based on policy
    QColor agentColor;
    if (m_policy == PolicyType::SHORTEST_PATH) {
        agentColor = QColor(168, 85, 247); // Purple
    } else {
        agentColor = QColor(16, 185, 129); // Green
    }
    
    // Selection glow
    if (m_selected) {
        QRadialGradient selectionGradient(0, 0, AGENT_RADIUS + 4);
        selectionGradient.setColorAt(0, QColor(agentColor.red(), agentColor.green(), agentColor.blue(), 150));
        selectionGradient.setColorAt(1, QColor(agentColor.red(), agentColor.green(), agentColor.blue(), 0));
        painter->setBrush(selectionGradient);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QRectF(-AGENT_RADIUS - 4, -AGENT_RADIUS - 4, 
                                   2 * (AGENT_RADIUS + 4), 2 * (AGENT_RADIUS + 4)));
    }
    
    // Agent circle
    QRadialGradient agentGradient(0, 0, AGENT_RADIUS);
    agentGradient.setColorAt(0, agentColor.lighter(120));
    agentGradient.setColorAt(1, agentColor);
    
    painter->setBrush(agentGradient);
    painter->setPen(QPen(agentColor.lighter(150), 1.5));
    painter->drawEllipse(QRectF(-AGENT_RADIUS, -AGENT_RADIUS, 
                               2 * AGENT_RADIUS, 2 * AGENT_RADIUS));
}

void AgentGraphicsItem::setPosition(const QPointF& pos) {
    m_position = pos;
    setPos(pos);
}

void AgentGraphicsItem::addTrailPoint(const QPointF& pos, const QColor& color) {
    TrailPoint point;
    point.pos = pos;
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
        point.opacity = qMax(0.0, point.opacity - 0.05);
    }
    
    // Remove fully faded points
    m_trail.erase(std::remove_if(m_trail.begin(), m_trail.end(),
                                 [](const TrailPoint& p) { return p.opacity <= 0.0; }),
                 m_trail.end());
}

void AgentGraphicsItem::setSelected(bool selected) {
    m_selected = selected;
    update();
}

void AgentGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_agent) {
        // Only allow selection if agent is still valid
        setSelected(true);
        event->accept();
    }
    QGraphicsItem::mousePressEvent(event);
}

// ============================================================================
// GridView Implementation
// ============================================================================

GridView::GridView(QWidget* parent)
    : QGraphicsView(parent), m_controller(nullptr), m_scene(new QGraphicsScene(this)),
      m_zoomLevel(1.0), m_showGrid(true), m_showNodeLabels(false),
      m_panning(false), m_animationTimer(new QTimer(this)), m_selectedAgent(nullptr) {
    
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setBackgroundBrush(QColor(10, 10, 10)); // Dark background #0A0A0A
    setStyleSheet("background-color: #0A0A0A; border-radius: 8px;");
    
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
    if (!m_controller || !m_controller->getCity()) {
        return;
    }
    
    City* city = m_controller->getCity();
    
    // Clear existing items
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
    
    // Clear selected agent when scene is updated
    m_selectedAgent = nullptr;
    
    // Create node items
    createNodeItems();
    
    // Create edge items
    createEdgeItems();
    
    // Create agent items
    createAgentItems();
    
    // Update colors
    updateEdgeColors();
    
    // Fit to window initially
    fitToWindow();
}

void GridView::createNodeItems() {
    if (!m_controller || !m_controller->getCity()) return;
    
    City* city = m_controller->getCity();
    
    // Find max row/col to determine grid size
    int maxRow = 0, maxCol = 0;
    int nodeCount = city->getNodeCount();
    for (int i = 0; i < nodeCount; ++i) {
        NodeId nodeId = city->getNodeIdByIndex(i);
        const Node& node = city->getNode(nodeId);
        maxRow = std::max(maxRow, node.getRow());
        maxCol = std::max(maxCol, node.getCol());
    }
    
    // Calculate spacing (assuming grid layout)
    const qreal SPACING = 80.0;
    qreal startX = 100;
    qreal startY = 100;
    
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
    
    int edgeCount = city->getEdgeCount();
    for (int i = 0; i < edgeCount; ++i) {
        EdgeId edgeId = city->getEdgeIdByIndex(i);
        const Edge& edge = city->getEdge(edgeId);
        QPointF fromPos = nodePosition(edge.getFrom());
        QPointF toPos = nodePosition(edge.getTo());
        
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
    
    for (auto it = m_edgeItems.begin(); it != m_edgeItems.end(); ++it) {
        EdgeId edgeId = it.key();
        EdgeGraphicsItem* item = it.value();
        
        const Edge& edge = city->getEdge(edgeId);
        int occupancy = city->occupancy(edgeId);
        int capacity = edge.getCapacity();
        double length = edge.getLength();
        
        item->setCongestion(occupancy, capacity, length);
        
        // Animate color transition
        QColor targetColor = getCongestionColor(occupancy, capacity);
        
        if (m_edgeColorAnimations.contains(item)) {
            QPropertyAnimation* anim = m_edgeColorAnimations[item];
            anim->stop();
            anim->setEndValue(targetColor);
            anim->start();
        } else {
            QPropertyAnimation* anim = new QPropertyAnimation(item, "color");
            anim->setDuration(300);
            anim->setEasingCurve(QEasingCurve::InOutQuad);
            anim->setStartValue(item->currentColor());
            anim->setEndValue(targetColor);
            m_edgeColorAnimations[item] = anim;
            anim->start();
        }
    }
}

QColor GridView::getCongestionColor(int occupancy, int capacity) const {
    double ratio = capacity > 0 ? static_cast<double>(occupancy) / capacity : 0.0;
    
    if (ratio == 0.0) {
        return QColor(59, 130, 246); // Cool blue - Empty
    } else if (ratio < 0.3) {
        return QColor(6, 182, 212); // Cyan - Light traffic
    } else if (ratio < 0.7) {
        return QColor(245, 158, 11); // Yellow - Medium
    } else if (ratio < 1.0) {
        return QColor(249, 115, 22); // Orange - Heavy
    } else {
        return QColor(239, 68, 68); // Red - Gridlock
    }
}

void GridView::updateAgentPositions() {
    if (!m_controller) return;
    
    auto& agents = m_controller->getAgents();
    
    // Build a set of valid agent pointers for quick lookup
    QSet<Agent*> validAgents;
    for (Agent* agent : agents) {
        if (agent) {
            validAgents.insert(agent);
        }
    }
    
    // Remove items for agents that no longer exist
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
    
    for (Agent* agent : agents) {
        if (!agent || !m_agentItems.contains(agent)) continue;
        
        AgentGraphicsItem* item = m_agentItems[agent];
        
        QPointF targetPos;
        if (agent->getCurrentEdge().has_value()) {
            // Agent is on an edge - interpolate position
            EdgeId edgeId = agent->getCurrentEdge().value();
            const Edge& edge = m_controller->getCity()->getEdge(edgeId);
            // For now, assume halfway (we'd need to track progress in Agent)
            targetPos = interpolatePosition(edge.getFrom(), edge.getTo(), 0.5);
        } else {
            // Agent is at a node
            targetPos = nodePosition(agent->getCurrentNode());
        }
        
        // Animate position
        QPointF currentPos = item->position();
        if (currentPos != targetPos) {
            if (m_agentAnimations.contains(item)) {
                QPropertyAnimation* anim = m_agentAnimations[item];
                anim->stop();
                anim->setStartValue(currentPos);
                anim->setEndValue(targetPos);
                anim->start();
            } else {
                QPropertyAnimation* anim = new QPropertyAnimation(item, "pos");
                anim->setDuration(200);
                anim->setEasingCurve(QEasingCurve::InOutQuad);
                anim->setStartValue(currentPos);
                anim->setEndValue(targetPos);
                m_agentAnimations[item] = anim;
                anim->start();
            }
        }
        
        // Update trail
        QColor trailColor = (m_controller->getPolicy() == PolicyType::CONGESTION_AWARE ? 
                            QColor(16, 185, 129) : QColor(168, 85, 247));
        item->addTrailPoint(targetPos, trailColor);
    }
}

void GridView::animateStep() {
    updateEdgeColors();
    updateAgentPositions();
    
    // Update trails
    for (auto* item : m_agentItems.values()) {
        item->updateTrail();
    }
}

void GridView::onAnimationFrame() {
    // Update trail fading
    for (auto* item : m_agentItems.values()) {
        item->updateTrail();
    }
    
    // Update edge color animations
    for (auto* anim : m_edgeColorAnimations.values()) {
        if (anim->state() == QPropertyAnimation::Running) {
            // Animation updates automatically
        }
    }
    
    viewport()->update();
}

void GridView::fitToWindow() {
    if (m_scene->items().isEmpty()) return;
    
    QRectF itemsRect = m_scene->itemsBoundingRect();
    if (itemsRect.isEmpty()) return;
    
    itemsRect.adjust(-50, -50, 50, 50); // Add padding
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

void GridView::setZoomLevel(qreal level) {
    level = qBound(1.0, level, 5.0);
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
    
    // Handle agent selection
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());
        AgentGraphicsItem* agentItem = qgraphicsitem_cast<AgentGraphicsItem*>(item);
        
        // Deselect previous agent
        if (m_selectedAgent && m_selectedAgent != agentItem) {
            m_selectedAgent->setSelected(false);
        }
        
        // Select new agent
        if (agentItem) {
            Agent* agent = agentItem->agent();
            // Verify agent is still valid by checking if it exists in current agents list
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
            } else {
                m_selectedAgent = nullptr;
                emit agentSelected(nullptr);
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
    const int step = 20;
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
    Q_UNUSED(rect);
    
    // Dark background #0A0A0A
    painter->fillRect(rect, QColor(10, 10, 10));
    
    if (m_showGrid) {
        // Draw subtle grid with #3E3E42 color
        painter->setPen(QPen(QColor(62, 62, 66), 1));
        const qreal gridSize = 50.0;
        
        qreal left = qFloor(rect.left() / gridSize) * gridSize;
        qreal top = qFloor(rect.top() / gridSize) * gridSize;
        
        for (qreal x = left; x < rect.right(); x += gridSize) {
            painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
        }
        
        for (qreal y = top; y < rect.bottom(); y += gridSize) {
            painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
        }
    }
}

void GridView::updateAgentTrail(AgentGraphicsItem* agentItem, Agent* agent) {
    // This is handled in updateAgentPositions now
    Q_UNUSED(agentItem);
    Q_UNUSED(agent);
}

void GridView::clearAgentTrails() {
    for (auto* item : m_agentItems.values()) {
        // Clear trail by removing all points
        // This would need a method in AgentGraphicsItem
    }
}

