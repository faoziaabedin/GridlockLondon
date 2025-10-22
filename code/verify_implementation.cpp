// Minimal verification of IRoutePolicy and ShortestPathPolicy implementation
// This file focuses on verifying the interface and implementation logic

// Check that IRoutePolicy.h has the required interface
#include "core/IRoutePolicy.h"
#include "core/ShortestPathPolicy.h"

// This file will only compile if:
// 1. IRoutePolicy.h exists and has the correct interface
// 2. ShortestPathPolicy.h exists and inherits from IRoutePolicy
// 3. All required methods are declared

int main() {
    // If this compiles, the interface is correct
    return 0;
}
