// code/patterns/RandomGridFactory.h
#pragma once

#include "IGridFactory.h"
#include <random>

/**
 * Factory Pattern: Concrete Factory - Random Grid
 * 
 * Creates grid topologies with random connections:
 * - Nodes arranged in a rectangular grid
 * - Random subset of possible edges (not all neighbors connected)
 * - Variable edge lengths and capacities
 * - Useful for testing pathfinding with incomplete graphs
 */
class RandomGridFactory : public IGridFactory {
public:
    /**
     * @param connectionProbability Probability (0.0-1.0) that any potential edge exists
     * @param seed Random seed for reproducibility
     */
    explicit RandomGridFactory(double connectionProbability = 0.7, unsigned int seed = 42);
    ~RandomGridFactory() override = default;
    
    std::unique_ptr<City> createGrid(int rows, int cols) override;
    std::string getFactoryType() const override;
    
private:
    double connectionProbability;
    std::mt19937 rng;
    std::uniform_real_distribution<double> probDist;
    std::uniform_real_distribution<double> lengthDist;
    std::uniform_int_distribution<int> capacityDist;
    
    void addBidirectionalEdge(City& city, int& edgeId, NodeId from, NodeId to,
                              double length, int capacity);
};

