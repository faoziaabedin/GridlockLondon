#include "RoutePlanner.h"
#include "City.h"
#include "Agent.h"
#include "Types.h"
#include <algorithm>
#include <climits>
#include <unordered_map>
#include <queue>
#include <utility>

// Custom hash for pair<NodeId, NodeId>
namespace std {
    template <>
    struct hash<std::pair<NodeId, NodeId>> {
        size_t operator()(const std::pair<NodeId, NodeId>& p) const noexcept {
            auto h1 = std::hash<NodeId>{}(p.first);
            auto h2 = std::hash<NodeId>{}(p.second);
            // Combine the two hashes safely
            return h1 ^ (h2 << 1);
        }
    };
}

RoutePlanner::RoutePlanner(IRoutePolicy* p) : policy(p) {
    // Constructor initializes the policy pointer
}

void RoutePlanner::setPolicy(IRoutePolicy* p) {
    policy = p;
}

std::deque<EdgeId> RoutePlanner::computePath(City& city, Agent& agent) {
    if (!policy) {
        return std::deque<EdgeId>(); // Return empty path if no policy
    }

    NodeId start = agent.getCurrentNode();
    NodeId goal = agent.getDestination();

    if (start == goal) {
        return std::deque<EdgeId>();
    }

    return dijkstra(city, start, goal);
}

std::deque<EdgeId> RoutePlanner::dijkstra(City& city, NodeId start, NodeId goal) {
    if (!policy) {
        return std::deque<EdgeId>();
    }

    // Distance map: node -> shortest distance from start
    std::unordered_map<NodeId, double> distances;

    // Predecessor map: node -> previous node
    std::unordered_map<NodeId, NodeId> predecessors;

    // Edge map: (from, to) -> edgeId for path reconstruction
    std::unordered_map<std::pair<NodeId, NodeId>, EdgeId, std::hash<std::pair<NodeId, NodeId>>> edgeMap;

    // Priority queue for Dijkstra: (distance, node)
    std::priority_queue<std::pair<double, NodeId>,
                        std::vector<std::pair<double, NodeId>>,
                        std::greater<std::pair<double, NodeId>>> pq;

    distances[start] = 0.0;
    pq.push({0.0, start});

    while (!pq.empty()) {
        auto [currentDist, currentNode] = pq.top();
        pq.pop();

        if (currentDist > distances[currentNode]) continue;
        if (currentNode == goal) break;

        std::vector<EdgeId> outgoingEdges = city.neighbors(currentNode);

        for (EdgeId edgeId : outgoingEdges) {
            const Edge& edge = city.getEdge(edgeId);
            NodeId neighbor = edge.getTo();

            if (edge.isBlocked()) continue;

            double edgeCost = policy->edgeCost(city, edgeId);
            double newDist = currentDist + edgeCost;

            if (distances.find(neighbor) == distances.end() || newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                predecessors[neighbor] = currentNode;
                edgeMap[{currentNode, neighbor}] = edgeId;
                pq.push({newDist, neighbor});
            }
        }
    }

    if (distances.find(goal) == distances.end()) {
        return std::deque<EdgeId>(); // No path
    }

    return reconstructPath(predecessors, edgeMap, start, goal);
}

std::deque<EdgeId> RoutePlanner::reconstructPath(
    const std::unordered_map<NodeId, NodeId>& predecessors,
    const std::unordered_map<std::pair<NodeId, NodeId>, EdgeId,
                             std::hash<std::pair<NodeId, NodeId>>>& edgeMap,
    NodeId start, NodeId goal) {

    std::deque<EdgeId> path;
    NodeId current = goal;

    while (current != start) {
        auto predIt = predecessors.find(current);
        if (predIt == predecessors.end()) return std::deque<EdgeId>();

        NodeId previous = predIt->second;

        auto edgeIt = edgeMap.find({previous, current});
        if (edgeIt == edgeMap.end()) return std::deque<EdgeId>();

        EdgeId edgeId = edgeIt->second;
        path.push_front(edgeId);
        current = previous;
    }

    return path;
}
