// code/tests/mocks/MockCity.cpp
#include "MockCity.h"
#include "../../core/City.h"
#include "../../core/Node.h"
#include "../../core/Edge.h"
#include "../../adapters/PresetLoader.h"

std::unique_ptr<City> TestCityBuilder::createSimpleGrid(int rows, int cols) {
    PresetLoader loader;
    return loader.createGridTopology(rows, cols);
}

std::unique_ptr<City> TestCityBuilder::createCityWithBlockedEdges(int rows, int cols,
                                                                   const std::vector<std::pair<NodeId, NodeId>>& blocked) {
    PresetLoader loader;
    auto city = loader.createGridTopology(rows, cols);
    
    // Apply blocked edges
    for (const auto& [from, to] : blocked) {
        // Find and remove the edge
        for (int i = 0; i < city->getEdgeCount(); ++i) {
            EdgeId eid = city->getEdgeIdByIndex(i);
            const Edge& edge = city->getEdge(eid);
            if ((edge.getFrom() == from && edge.getTo() == to) ||
                (edge.getFrom() == to && edge.getTo() == from)) {
                // Mark as blocked
                city->getEdge(eid).setBlocked(true);
                break;
            }
        }
    }
    
    return city;
}

std::unique_ptr<City> TestCityBuilder::createDisconnectedCity() {
    auto city = std::make_unique<City>();
    
    // Create two separate nodes with no connection
    city->addNode(Node(0, 0, 0));
    city->addNode(Node(1, 10, 10));
    
    return city;
}

