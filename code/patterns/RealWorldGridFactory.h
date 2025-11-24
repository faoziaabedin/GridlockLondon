// code/patterns/RealWorldGridFactory.h
#pragma once

#include "IGridFactory.h"
#include <string>
#include <vector>

/**
 * Factory Pattern: Concrete Factory - Real-World Grid
 * 
 * Creates grid topologies based on real-world data:
 * - Can load from GeoJSON or similar formats
 * - Supports custom node positions and edge configurations
 * - Useful for simulating actual city layouts
 * 
 * For this implementation, we create a highway-style network
 * with major roads (high capacity) and local roads (low capacity).
 */
class RealWorldGridFactory : public IGridFactory {
public:
    RealWorldGridFactory() = default;
    ~RealWorldGridFactory() override = default;
    
    std::unique_ptr<City> createGrid(int rows, int cols) override;
    std::string getFactoryType() const override;
    
    /**
     * Load grid from external data source (stub for future implementation).
     * In a full implementation, this would parse GeoJSON or similar.
     */
    void loadFromFile(const std::string& filepath);
    
private:
    /**
     * Create a highway-style network with major arteries.
     */
    void addHighwayNetwork(City& city, int rows, int cols, int& edgeId);
    
    /**
     * Add local road connections.
     */
    void addLocalRoads(City& city, int rows, int cols, int& edgeId);
    
    void addBidirectionalEdge(City& city, int& edgeId, NodeId from, NodeId to,
                               double length, int capacity);
};

