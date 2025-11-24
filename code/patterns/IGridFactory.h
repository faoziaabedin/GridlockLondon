// code/patterns/IGridFactory.h
#pragma once

#include <memory>
#include "../core/City.h"

/**
 * Factory Pattern: Abstract Factory Interface
 * 
 * Defines the interface for creating different grid topologies.
 * This allows us to create various grid types (regular, random, real-world)
 * without coupling the client code to specific implementations.
 */
class IGridFactory {
public:
    virtual ~IGridFactory() = default;
    
    /**
     * Create a grid topology with the specified dimensions.
     * @param rows Number of rows in the grid
     * @param cols Number of columns in the grid
     * @return Unique pointer to a City with the grid topology
     */
    virtual std::unique_ptr<City> createGrid(int rows, int cols) = 0;
    
    /**
     * Get the name/type of this factory.
     * @return String identifier for the factory type
     */
    virtual std::string getFactoryType() const = 0;
};

