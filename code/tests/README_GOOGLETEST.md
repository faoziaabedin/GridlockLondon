# GoogleTest/GoogleMock Test Suites

## Overview

This directory contains comprehensive test suites using GoogleTest and GoogleMock, targeting >85% code coverage with 100% coverage on critical paths.

## Test Suites

### 1. RoutePlanner Test Suite (15+ tests)
**File:** `test_route_planner_googletest.cpp`

Tests pathfinding, policy integration, edge cases, and performance.

**Key Tests:**
- Finds shortest path in simple grid
- Handles blocked edges
- Reroutes when capacity full
- Uses correct policy (with MockPolicy)
- Handles disconnected graph
- Performance benchmarks
- Parameterized tests for grid sizes

**Run:**
```bash
cd build
./test_route_planner_googletest
```

### 2. Agent Test Suite (12+ tests)
**File:** `test_agent_googletest.cpp`

Tests movement, capacity constraints, rerouting, and edge cases.

**Key Tests:**
- Agent creation and movement
- Capacity constraint handling
- Rerouting logic
- Edge cases (stuck agent, blocked destination)
- Travel time tracking
- Parameterized tests for agent configurations

**Run:**
```bash
cd build
./test_agent_googletest
```

### 3. Metrics Test Suite (10+ tests)
**File:** `test_metrics_googletest.cpp`

Tests calculation accuracy, edge cases, and performance.

**Key Tests:**
- Average trip time calculation
- Total throughput
- Max edge load tracking
- Edge load snapshots
- Reset functionality
- Performance with many agents

**Run:**
```bash
cd build
./test_metrics_googletest
```

### 4. SimulationController Test Suite (10+ tests)
**File:** `test_simulation_controller_googletest.cpp`

Tests state management, policy switching, reset, and multi-agent coordination.

**Key Tests:**
- State management (start/pause/reset)
- Policy switching
- Multi-agent coordination
- Metrics updates
- Singleton access
- Parameterized tests for policy types

**Run:**
```bash
cd build
./test_simulation_controller_googletest
```

### 5. Factory Pattern Test Suite (8+ tests)
**File:** `test_factory_pattern_googletest.cpp`

Tests grid generation, validation, and edge case grids.

**Key Tests:**
- Grid generation (various sizes)
- City building from preset
- Agent spawning
- Blocked edges application
- Edge cases (1x1, 2x2 grids)
- Parameterized tests for grid sizes

**Run:**
```bash
cd build
./test_factory_pattern_googletest
```

## Mock Objects

### MockPolicy
**File:** `mocks/MockPolicy.h`

GoogleMock implementation of `IRoutePolicy` for testing policy interactions.

**Usage:**
```cpp
MockPolicy mockPolicy;
EXPECT_CALL(mockPolicy, edgeCost(_, _))
    .WillRepeatedly(Return(1.0));
RoutePlanner planner(&mockPolicy);
```

### TestCityBuilder
**File:** `mocks/MockCity.h`, `mocks/MockCity.cpp`

Helper class to create test cities with various configurations.

**Methods:**
- `createSimpleGrid(rows, cols)` - Creates a standard grid
- `createCityWithBlockedEdges(rows, cols, blocked)` - Creates grid with blocked edges
- `createDisconnectedCity()` - Creates disconnected graph

## Running All Tests

### Using CTest
```bash
cd build
ctest
```

### Individual Test Suites
```bash
cd build
./test_route_planner_googletest
./test_agent_googletest
./test_metrics_googletest
./test_simulation_controller_googletest
./test_factory_pattern_googletest
```

### With Verbose Output
```bash
cd build
ctest --verbose
```

## Coverage Reporting

### Generate Coverage Report
```bash
cd code
./run_coverage.sh
```

Or manually:
```bash
cd build
cmake .. -DENABLE_COVERAGE=ON
make
make coverage
```

### View Coverage Report
Open `build/coverage_html/index.html` in your browser.

## Test Statistics

- **Total Test Cases:** 50+
- **Test Suites:** 5
- **Parameterized Tests:** 3
- **Mock Objects:** 2
- **Performance Tests:** 3
- **Edge Case Tests:** 10+

## Requirements

- GoogleTest/GoogleMock (automatically downloaded via CMake FetchContent)
- CMake 3.14+
- C++20 compiler
- lcov and genhtml (for coverage reports)

## Building

Tests are automatically built when you build the project:

```bash
cd code/build
cmake ..
make
```

GoogleTest will be automatically downloaded and built.

## Best Practices

1. **Use fixtures** for common setup/teardown
2. **Use parameterized tests** for testing multiple scenarios
3. **Use mocks** to isolate units under test
4. **Test edge cases** (empty inputs, boundary values, error conditions)
5. **Include performance tests** for critical paths
6. **Document test purpose** in comments

## Coverage Goals

- **Overall:** >85%
- **Critical Paths:** 100%
  - RoutePlanner::computePath()
  - Agent::step()
  - Metrics calculations
  - SimulationController state management

