// code/patterns/RegularGridFactory.cpp
#include "RegularGridFactory.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"

std::unique_ptr<City> RegularGridFactory::createGrid(int rows, int cols) {
    auto city = std::make_unique<City>();
    
    // Create nodes in a rectangular grid
    // Node ID = row * cols + col
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId nodeId = row * cols + col;
            city->addNode(Node(nodeId, row, col));
        }
    }
    
    int edgeId = 0;
    
    // Create horizontal edges (left-right connections)
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols - 1; ++col) {
            NodeId from = row * cols + col;
            NodeId to = row * cols + col + 1;
            addBidirectionalEdge(*city, edgeId, from, to);
        }
    }
    
    // Create vertical edges (top-bottom connections)
    for (int row = 0; row < rows - 1; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId from = row * cols + col;
            NodeId to = (row + 1) * cols + col;
            addBidirectionalEdge(*city, edgeId, from, to);
        }
    }
    
    return city;
}

void RegularGridFactory::addBidirectionalEdge(City& city, int& edgeId, 
                                               NodeId from, NodeId to,
                                               double length, int capacity) {
    // Forward edge
    Edge forwardEdge(edgeId++, from, to, length, capacity);
    city.addEdge(forwardEdge);
    
    // Reverse edge (bidirectional)
    Edge reverseEdge(edgeId++, to, from, length, capacity);
    city.addEdge(reverseEdge);
}

std::string RegularGridFactory::getFactoryType() const {
    return "RegularGrid";
}

