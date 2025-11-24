// code/patterns/RealWorldGridFactory.cpp
#include "RealWorldGridFactory.h"
#include "../core/City.h"
#include "../core/Node.h"
#include "../core/Edge.h"

std::unique_ptr<City> RealWorldGridFactory::createGrid(int rows, int cols) {
    auto city = std::make_unique<City>();
    
    // Create nodes
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId nodeId = row * cols + col;
            city->addNode(Node(nodeId, row, col));
        }
    }
    
    int edgeId = 0;
    
    // Create highway network (major arteries with high capacity)
    addHighwayNetwork(*city, rows, cols, edgeId);
    
    // Add local roads (lower capacity, more connections)
    addLocalRoads(*city, rows, cols, edgeId);
    
    return city;
}

void RealWorldGridFactory::addHighwayNetwork(City& city, int rows, int cols, int& edgeId) {
    // Horizontal highways (every 2 rows)
    for (int row = 1; row < rows; row += 2) {
        for (int col = 0; col < cols - 1; ++col) {
            NodeId from = row * cols + col;
            NodeId to = row * cols + col + 1;
            // Highways have longer length (distance) but high capacity
            addBidirectionalEdge(city, edgeId, from, to, 2.0, 20);
        }
    }
    
    // Vertical highways (every 2 columns)
    for (int col = 1; col < cols; col += 2) {
        for (int row = 0; row < rows - 1; ++row) {
            NodeId from = row * cols + col;
            NodeId to = (row + 1) * cols + col;
            addBidirectionalEdge(city, edgeId, from, to, 2.0, 20);
        }
    }
}

void RealWorldGridFactory::addLocalRoads(City& city, int rows, int cols, int& edgeId) {
    // Fill in remaining connections with local roads
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols - 1; ++col) {
            NodeId from = row * cols + col;
            NodeId to = row * cols + col + 1;
            
            // Check if edge doesn't already exist (from highway)
            bool exists = false;
            // Simple check: if it's a highway row, skip (already added)
            if (row % 2 == 1) {
                exists = true;
            }
            
            if (!exists) {
                addBidirectionalEdge(city, edgeId, from, to, 1.0, 10);
            }
        }
    }
    
    for (int row = 0; row < rows - 1; ++row) {
        for (int col = 0; col < cols; ++col) {
            NodeId from = row * cols + col;
            NodeId to = (row + 1) * cols + col;
            
            bool exists = (col % 2 == 1);
            if (!exists) {
                addBidirectionalEdge(city, edgeId, from, to, 1.0, 10);
            }
        }
    }
}

void RealWorldGridFactory::addBidirectionalEdge(City& city, int& edgeId,
                                                NodeId from, NodeId to,
                                                double length, int capacity) {
    Edge forwardEdge(edgeId++, from, to, length, capacity);
    city.addEdge(forwardEdge);
    
    Edge reverseEdge(edgeId++, to, from, length, capacity);
    city.addEdge(reverseEdge);
}

std::string RealWorldGridFactory::getFactoryType() const {
    return "RealWorldGrid";
}

void RealWorldGridFactory::loadFromFile(const std::string& filepath) {
    // Stub for future implementation
    // Would parse GeoJSON or similar format
}

