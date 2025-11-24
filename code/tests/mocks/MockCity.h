// code/tests/mocks/MockCity.h
#pragma once

#include <gmock/gmock.h>
#include "../../core/City.h"

/**
 * Mock implementation of City for testing.
 * Note: Since City is a concrete class, we'll use a wrapper approach
 * or test with real City instances. For now, we'll create helper functions.
 */

// Helper class to create test cities
class TestCityBuilder {
public:
    static std::unique_ptr<City> createSimpleGrid(int rows, int cols);
    static std::unique_ptr<City> createCityWithBlockedEdges(int rows, int cols, 
                                                             const std::vector<std::pair<NodeId, NodeId>>& blocked);
    static std::unique_ptr<City> createDisconnectedCity();
};

