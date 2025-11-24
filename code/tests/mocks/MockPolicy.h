// code/tests/mocks/MockPolicy.h
#pragma once

#include <gmock/gmock.h>
#include "../../core/IRoutePolicy.h"

/**
 * Mock implementation of IRoutePolicy for testing.
 * Uses GoogleMock to verify policy interactions.
 */
class MockPolicy : public IRoutePolicy {
public:
    MOCK_METHOD(double, edgeCost, (const City&, EdgeId), (const, override));
    MOCK_METHOD(bool, shouldRerouteOnNode, (const Agent&), (const, override));
    
    // Default implementations for convenience
    MockPolicy() {
        ON_CALL(*this, edgeCost)
            .WillByDefault(testing::Return(1.0));
        ON_CALL(*this, shouldRerouteOnNode)
            .WillByDefault(testing::Return(false));
    }
};

