// code/patterns/RegularGridFactory.h
#pragma once

#include "IGridFactory.h"

/**
 * Factory Pattern: Concrete Factory - Regular Grid
 * 
 * Creates standard NxM grid topologies with:
 * - Nodes arranged in a rectangular grid
 * - Bidirectional edges connecting horizontal and vertical neighbors
 * - Uniform edge lengths and capacities
 */
class RegularGridFactory : public IGridFactory {
public:
    RegularGridFactory() = default;
    ~RegularGridFactory() override = default;
    
    std::unique_ptr<City> createGrid(int rows, int cols) override;
    std::string getFactoryType() const override;
    
private:
    /**
     * Helper to create bidirectional edge pair.
     */
    void addBidirectionalEdge(City& city, int& edgeId, NodeId from, NodeId to, 
                              double length = 1.0, int capacity = 10);
};

