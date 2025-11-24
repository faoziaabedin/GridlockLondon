// code/patterns/RandomGridFactory.cpp
#include "RandomGridFactory.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"

RandomGridFactory::RandomGridFactory(double connectionProbability, unsigned int seed)
    : connectionProbability(connectionProbability),
      rng(seed),
      probDist(0.0, 1.0),
      lengthDist(0.5, 2.0),  // Variable edge lengths
      capacityDist(5, 15) {  // Variable capacities
}

std::unique_ptr<City> RandomGridFactory::createGrid(int rows, int cols) {
    auto city = std::make_unique<City>();
    
    // Create nodes in a rectangular grid
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId nodeId = row * cols + col;
            city->addNode(Node(nodeId, row, col));
        }
    }
    
    int edgeId = 0;
    
    // Randomly create horizontal edges
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols - 1; ++col) {
            if (probDist(rng) < connectionProbability) {
                NodeId from = row * cols + col;
                NodeId to = row * cols + col + 1;
                double length = lengthDist(rng);
                int capacity = capacityDist(rng);
                addBidirectionalEdge(*city, edgeId, from, to, length, capacity);
            }
        }
    }
    
    // Randomly create vertical edges
    for (int row = 0; row < rows - 1; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (probDist(rng) < connectionProbability) {
                NodeId from = row * cols + col;
                NodeId to = (row + 1) * cols + col;
                double length = lengthDist(rng);
                int capacity = capacityDist(rng);
                addBidirectionalEdge(*city, edgeId, from, to, length, capacity);
            }
        }
    }
    
    return city;
}

void RandomGridFactory::addBidirectionalEdge(City& city, int& edgeId,
                                             NodeId from, NodeId to,
                                             double length, int capacity) {
    Edge forwardEdge(edgeId++, from, to, length, capacity);
    city.addEdge(forwardEdge);
    
    Edge reverseEdge(edgeId++, to, from, length, capacity);
    city.addEdge(reverseEdge);
}

std::string RandomGridFactory::getFactoryType() const {
    return "RandomGrid";
}

