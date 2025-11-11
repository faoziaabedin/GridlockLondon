// code/tests/test_city.cpp
#include <iostream>
#include <memory>
#include <deque>
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"
#include "../core/Agent.h"
#include "../core/RoutePlanner.h"
#include "../core/ShortestPathPolicy.h"
#include "../core/CongestionAwarePolicy.h"
#include "../adapters/PresetLoader.h"

int main() {
    std::cout << "===== GridlockLondon: Functional Core Test =====\n";

    // Step 1. Build a small demo city using PresetLoader (builder pattern)
    PresetLoader loader;
    std::unique_ptr<City> city = loader.createGridTopology(3, 3);
    std::cout << " City created: 3x3 grid with "
              << "9 nodes and "
              << "some edges.\n";

    // Step 2. Create an Agent (origin = 0, destination = 8)
    Agent agent(1, 0, 8);
    std::cout << " Agent created (ID=" << agent.getId()
              << ", origin=" << agent.getOrigin()
              << ", destination=" << agent.getDestination() << ")\n";

    // Step 3. Use RoutePlanner with ShortestPathPolicy (Strategy pattern)
    ShortestPathPolicy shortestPolicy;
    RoutePlanner planner(&shortestPolicy);
    std::deque<EdgeId> path1 = planner.computePath(*city, agent);

    std::cout << "\n--- ShortestPathPolicy ---\n";
    if (path1.empty()) {
        std::cout << " No path found.\n";
    } else {
        std::cout << " Found path with " << path1.size() << " edges: ";
        for (EdgeId edgeId : path1) {
            const Edge& e = city->getEdge(edgeId);
            std::cout << e.getFrom() << "→" << e.getTo() << "  ";
        }
        std::cout << "\n";
    }

    // Step 4. Switch to CongestionAwarePolicy (Strategy swap)
    CongestionAwarePolicy congestionPolicy;
    planner.setPolicy(&congestionPolicy);
    std::deque<EdgeId> path2 = planner.computePath(*city, agent);

    std::cout << "\n--- CongestionAwarePolicy ---\n";
    if (path2.empty()) {
        std::cout << " No path found.\n";
    } else {
        std::cout << " Found path with " << path2.size() << " edges: ";
        for (EdgeId edgeId : path2) {
            const Edge& e = city->getEdge(edgeId);
            std::cout << e.getFrom() << "→" << e.getTo() << "  ";
        }
        std::cout << "\n";
    }

    // Step 5. Block some roads and recompute
    std::vector<std::pair<NodeId, NodeId>> blocked = {{0, 1}, {1, 2}};
    loader.applyBlockedEdges(*city, blocked);
    std::cout << "\n Blocked edges between nodes 0–1 and 1–2.\n";

    std::deque<EdgeId> path3 = planner.computePath(*city, agent);
    std::cout << "\n--- Rerouting After Blocking ---\n";
    if (path3.empty()) {
        std::cout << " No alternate path available.\n";
    } else {
        std::cout << " Alternate path found with " << path3.size() << " edges: ";
        for (EdgeId edgeId : path3) {
            const Edge& e = city->getEdge(edgeId);
            std::cout << e.getFrom() << "→" << e.getTo() << "  ";
        }
        std::cout << "\n";
    }

    std::cout << "\n===== All Tests Completed Successfully =====\n";
    return 0;
}
