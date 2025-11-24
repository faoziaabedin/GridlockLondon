#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsItem>
#include <QMap>
#include <QSet>
#include <memory>
#include "../core/Types.h"
#include "../core/Preset.h"

// Forward declarations
class City;
class Agent;
class SimulationController;
class NodeGraphicsItem;
class EdgeGraphicsItem;
class AgentGraphicsItem;

/**
 * Modern grid view with smooth animations, zoom, pan, and interactive features.
 * Implements a professional dark-themed visualization.
 */
class GridView : public QGraphicsView {
    Q_OBJECT
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel)

public:
    explicit GridView(QWidget* parent = nullptr);
    ~GridView();

    // Setup and update
    void setSimulationController(SimulationController* controller);
    void updateScene();
    void animateStep();

    // View controls
    void fitToWindow();
    void setShowGrid(bool show);
    void setShowNodeLabels(bool show);
    qreal zoomLevel() const { return m_zoomLevel; }
    void setZoomLevel(qreal level);

    // Agent selection
    AgentGraphicsItem* selectedAgent() const { return m_selectedAgent; }

signals:
    void agentSelected(Agent* agent);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private slots:
    void onAnimationFrame();

private:
    void setupScene();
    void createNodeItems();
    void createEdgeItems();
    void createAgentItems();
    void updateEdgeColors();
    void updateAgentPositions();
    QColor getCongestionColor(int occupancy, int capacity) const;
    QPointF nodePosition(NodeId nodeId) const;
    QPointF interpolatePosition(NodeId from, NodeId to, qreal progress) const;
    void updateAgentTrail(AgentGraphicsItem* agentItem, Agent* agent);
    void clearAgentTrails();

    SimulationController* m_controller;
    QGraphicsScene* m_scene;
    
    // Graphics items
    QMap<NodeId, NodeGraphicsItem*> m_nodeItems;
    QMap<EdgeId, EdgeGraphicsItem*> m_edgeItems;
    QMap<Agent*, AgentGraphicsItem*> m_agentItems;
    
    // View state
    qreal m_zoomLevel;
    bool m_showGrid;
    bool m_showNodeLabels;
    bool m_panning;
    QPoint m_lastPanPoint;
    
    // Animation
    QTimer* m_animationTimer;
    QMap<AgentGraphicsItem*, QPropertyAnimation*> m_agentAnimations;
    QMap<EdgeGraphicsItem*, QPropertyAnimation*> m_edgeColorAnimations;
    
    // Agent selection
    AgentGraphicsItem* m_selectedAgent;
    
    // Agent trails (for fading effect)
    struct TrailPoint {
        QPointF pos;
        qreal opacity;
        QColor color;
    };
    QMap<AgentGraphicsItem*, QList<TrailPoint>> m_agentTrails;
};

/**
 * Graphics item for a node (intersection).
 * Circular with subtle glow effect.
 */
class NodeGraphicsItem : public QGraphicsItem {
public:
    NodeGraphicsItem(NodeId id, const QPointF& position, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    NodeId nodeId() const { return m_nodeId; }
    void setShowLabel(bool show);
    
private:
    NodeId m_nodeId;
    QPointF m_position;
    bool m_showLabel;
    static constexpr qreal NODE_RADIUS = 8.0;
    static constexpr qreal GLOW_RADIUS = 12.0;
};

/**
 * Graphics item for an edge (road segment).
 * Line with gradient colors based on congestion, with smooth color transitions.
 */
class EdgeGraphicsItem : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(QColor color READ currentColor WRITE setColor)

public:
    EdgeGraphicsItem(EdgeId id, NodeId from, NodeId to, 
                     const QPointF& fromPos, const QPointF& toPos,
                     QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    EdgeId edgeId() const { return m_edgeId; }
    void setColor(const QColor& color);
    QColor currentColor() const { return m_currentColor; }
    void setCongestion(int occupancy, int capacity, double length);
    
    // For tooltip
    QString tooltipText() const;
    
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    
private:
    EdgeId m_edgeId;
    NodeId m_fromNode;
    NodeId m_toNode;
    QPointF m_fromPos;
    QPointF m_toPos;
    QColor m_currentColor;
    QColor m_targetColor;
    int m_occupancy;
    int m_capacity;
    double m_length;
    bool m_hovered;
    static constexpr qreal EDGE_WIDTH = 3.0;
};

/**
 * Graphics item for an agent (vehicle).
 * Animated circle with trail effect and policy-based colors.
 */
class AgentGraphicsItem : public QGraphicsObject {
    Q_OBJECT
public:
    AgentGraphicsItem(Agent* agent, PolicyType policy, QGraphicsItem* parent = nullptr);
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    Agent* agent() const { return m_agent; }
    void setPosition(const QPointF& pos);
    QPointF position() const { return m_position; }
    void addTrailPoint(const QPointF& pos, const QColor& color);
    void updateTrail();
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    struct TrailPoint {
        QPointF pos;
        qreal opacity;
        QColor color;
    };
    
    Agent* m_agent;
    PolicyType m_policy;
    QPointF m_position;
    QList<TrailPoint> m_trail;
    bool m_selected;
    static constexpr qreal AGENT_RADIUS = 6.0;
    static constexpr int MAX_TRAIL_POINTS = 20;
};

