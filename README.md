[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/8Cy36LS2)
[![Work in MakeCode](https://classroom.github.com/assets/work-in-make-code-8824cc13a1a3f34ffcd245c82f0ae96fdae6b7d554b6539aec3a03a70825519c.svg)](https://classroom.github.com/online_ide?assignment_repo_id=21096241&assignment_repo_type=AssignmentRepo)
# Deliverable 3 – Final Project 

# GridlockLondon - Traffic Simulation System

**Authors:**  
- Faozia Abedin (251358251, fabedin4@uwo.ca)  
- Paridhi Mehla (251371402, pmehla2@uwo.ca)


## Project Overview

GridlockLondon simulates traffic flow on a grid network where agents (vehicles) navigate from origin to destination using different routing policies. The system models road capacity constraints, dynamic congestion, and provides real-time metrics for traffic analysis.

**Deliverable:** D3 - Final Project   
**Due Date:** December 19, 2025


## Build Instructions

### Clone the Repository
```bash
git clone <repository-url>
cd GridlockLondon
```

### Build with CMake
```bash
cd code
mkdir -p build
cd build
cmake ..
make
```

### Alternative: Qt Creator (Recommended)
1. Open Qt Creator
2. File → Open File or Project → Select `code/CMakeLists.txt`
3. Build → Build Project (or press Cmd+B / Ctrl+B)
4. Executables will be in the `build` directory

### Running the Application

After building, run the GUI application:
```bash
cd code/build
./gridlock_ui
```

The application provides:
- Interactive traffic simulation visualization
- Real-time metrics and analytics
- Policy comparison and analysis
- Preset scenario loading
- Export capabilities

## Project Structure

```
GridlockLondon/
├── code/
│   ├── CMakeLists.txt           # Build configuration
│   ├── main.cpp                 # CLI demo entry point
│   ├── core/                    # Domain logic
│   │   ├── City.h/.cpp          # Graph model + occupancy
│   │   ├── Agent.h/.cpp         # Mobile vehicle entity
│   │   ├── Node.h/.cpp          # Graph node representation
│   │   ├── Edge.h/.cpp          # Graph edge representation
│   │   ├── IRoutePolicy.h       # Strategy interface
│   │   ├── ShortestPathPolicy.h/.cpp
│   │   ├── CongestionAwarePolicy.h/.cpp
│   │   ├── RoutePlanner.h/.cpp  # Dijkstra pathfinding
│   │   ├── Metrics.h/.cpp       # KPI tracking
│   │   ├── Preset.h/.cpp        # Simulation preset configuration
│   │   └── SimulationController.h/.cpp
│   ├── adapters/                # I/O and persistence
│   │   ├── PresetLoader.h/.cpp  # JSON configuration loading
│   │   ├── JsonReader.h/.cpp    # JSON parsing utilities
│   │   ├── ReportWriter.h/.cpp  # Report generation
│   │   └── TimerService.h/.cpp  # Timing services
│   ├── patterns/                # Creational design patterns
│   │   ├── RegularGridFactory.h/.cpp
│   │   ├── RandomGridFactory.h/.cpp
│   │   ├── RealWorldGridFactory.h/.cpp
│   │   ├── PresetBuilder.h/.cpp
│   │   ├── ShortestPathFactory.h/.cpp
│   │   ├── CongestionAwareFactory.h/.cpp
│   │   ├── PolicyRegistry.h/.cpp
│   │   └── IGridFactory.h, IPolicyFactory.h
│   ├── analytics/               # Analytics and reporting
│   │   ├── TrafficFlowAnalyzer.h/.cpp
│   │   ├── PolicyEffectivenessAnalyzer.h/.cpp
│   │   ├── PredictiveAnalyzer.h/.cpp
│   │   └── ReportExporter.h/.cpp
│   ├── ui/                      # Presentation layer (Qt GUI)
│   │   ├── main.cpp             # Qt application entry point
│   │   ├── MainWindow.h/.cpp    # Main application window
│   │   ├── GridView.h/.cpp      # Traffic grid visualization
│   │   ├── MetricsPanel.h/.cpp  # Real-time metrics display
│   │   ├── AnalyticsPanel.h/.cpp # Analytics visualization
│   │   └── resources.qrc        # Qt resources
│   ├── tests/                   # Comprehensive test suites
│   │   ├── test_city.cpp
│   │   ├── test_agent_googletest.cpp
│   │   ├── test_route_planner_googletest.cpp
│   │   ├── test_metrics_googletest.cpp
│   │   ├── test_simulation_controller_googletest.cpp
│   │   ├── test_factory_pattern_googletest.cpp
│   │   ├── test_creational_patterns.cpp
│   │   ├── test_analytics.cpp
│   │   ├── test_grid_view.cpp
│   │   ├── test_main_window.cpp
│   │   ├── test_metrics_panel.cpp
│   │   ├── test_ui_integration.cpp
│   │   ├── test_modern_ui.cpp
│   │   └── mocks/               # Mock objects for testing
│   ├── presets/                 # JSON scenario configurations
│   │   ├── demo_3x3.json
│   │   ├── demo_5x5.json
│   │   ├── london_10x10.json
│   │   └── manhattan_15x15.json
│   ├── run_all_tests.sh         # Comprehensive test runner
│   ├── run_coverage.sh          # Code coverage script
│   ├── verify_tests.py          # Python verification scripts
│   ├── verify_route_planner.py
│   ├── verify_congestion_policy.py
│   └── final_verification.py
└── report/
    ├── Design_Rationale.pdf
    ├── Video_Walkthrough.mp4
    └── uml/                     # Class, use case, sequence diagrams
```

## Design Overview

### Architecture
The system uses **layered architecture** with four main layers:
- **Core**: Pure domain logic (graph model, agents, routing, metrics, simulation controller)
- **Adapters**: External I/O (JSON loading, timers, report generation)
- **Patterns**: Creational design patterns (factories, builders, registries)
- **Analytics**: Traffic analysis, policy effectiveness, predictive modeling
- **UI**: Presentation layer with Qt-based GUI featuring real-time visualization

### Design Patterns

**Strategy Pattern** (`IRoutePolicy`, `ShortestPathPolicy`, `CongestionAwarePolicy`)  
Allows runtime selection between routing algorithms without modifying client code.

**Factory Pattern** (`RegularGridFactory`, `RandomGridFactory`, `RealWorldGridFactory`)  
Encapsulates grid creation logic, enabling polymorphic grid generation.

**Builder Pattern** (`PresetBuilder`)  
Provides fluent interface for constructing complex preset configurations step-by-step.

**Abstract Factory Pattern** (`ShortestPathFactory`, `CongestionAwareFactory`, `PolicyRegistry`)  
Creates families of related policy objects with a registry for runtime policy selection.

**Facade Pattern** (`RoutePlanner`)  
Simplifies complex pathfinding operations behind a clean, unified interface.

**Observer Pattern** (`Metrics`)  
Tracks simulation events without tight coupling between simulation and metrics collection.

**Singleton Pattern** (`SimulationController`)  
Ensures a single instance of the simulation controller manages the entire simulation state.


## Testing

The project includes comprehensive test suites using multiple testing frameworks to ensure code quality and correctness.

### Test Frameworks

1. **Qt Test Framework**: Used for UI component testing
2. **GoogleTest/GoogleMock**: Used for unit testing and mocking
3. **Python Verification Scripts**: Used for integration and contract testing

### Running Tests

#### Quick Test (All Tests)
```bash
cd code
./run_all_tests.sh
```

This script runs:
- All C++ unit tests (Qt Test and GoogleTest)
- All Python verification scripts
- Provides a comprehensive test summary

#### Individual Test Suites

**Core Component Tests:**
```bash
cd code/build
./test_city                          # City graph model tests
./test_agent_googletest              # Agent movement and behavior (12+ tests)
./test_route_planner_googletest      # Pathfinding algorithms (15+ tests)
./test_metrics_googletest            # Metrics calculation (10+ tests)
./test_simulation_controller_googletest  # Simulation control (10+ tests)
```

**Design Pattern Tests:**
```bash
./test_creational_patterns           # Factory, Builder, Abstract Factory patterns (25+ tests)
./test_factory_pattern_googletest    # Factory pattern implementation (8+ tests)
```

**UI Component Tests:**
```bash
./test_grid_view                     # Grid visualization component
./test_main_window                   # Main window functionality
./test_metrics_panel                 # Metrics panel display
./test_ui_integration                # UI component integration
./test_modern_ui                     # Modern UI features
./test_ui_graphics_items             # Graphics items rendering
```

**Analytics Tests:**
```bash
./test_analytics                     # Analytics module tests (25+ tests)
```

**Python Verification:**
```bash
cd code
python3 verify_tests.py              # General implementation verification
python3 verify_route_planner.py      # Route planner contract verification
python3 verify_congestion_policy.py  # Congestion policy verification
python3 final_verification.py        # Final implementation check
```

#### Using CTest
```bash
cd code/build
ctest                                # Run all registered GoogleTest suites
ctest --output-on-failure            # Show detailed output for failures
```

### Test Coverage

The test suite includes:
- **100+ test cases** covering all major components
- Unit tests for core logic (agents, routing, metrics)
- Integration tests for component interaction
- UI tests for Qt components
- Mock-based testing for isolated component testing
- Parameterized tests for edge cases and various configurations

## Code Coverage

The project includes comprehensive code coverage reporting using `lcov` and `gcov`.

### Enabling Coverage

#### Method 1: Using the Coverage Script
```bash
cd code
./run_coverage.sh
```

This script:
1. Cleans previous coverage data
2. Configures CMake with `ENABLE_COVERAGE=ON`
3. Builds the project with coverage flags
4. Runs all tests
5. Generates HTML coverage report

#### Method 2: Manual Coverage Setup
```bash
cd code/build
cmake .. -DENABLE_COVERAGE=ON
make -j4
ctest --output-on-failure
```

### Generating Coverage Report

After building with coverage and running tests:

```bash
# Generate coverage.info file
lcov --directory . --capture --output-file coverage.info

# Remove system headers and external code
lcov --remove coverage.info \
    '/usr/*' \
    '*/googletest/*' \
    '*/build/*' \
    '*/tests/*' \
    --output-file coverage.info

# Generate HTML report
genhtml coverage.info --output-directory coverage_html
```

### Viewing Coverage Report

Open the generated HTML report in your browser:
```bash
open code/build/coverage_html/index.html    # macOS
xdg-open code/build/coverage_html/index.html  # Linux
```

The coverage report provides:
- Line-by-line coverage for each source file
- Function-level coverage statistics
- Branch coverage information
- Overall project coverage percentage

### Coverage Summary

View a summary without opening the HTML report:
```bash
cd code/build
lcov --summary coverage.info
```

### Requirements

Coverage reporting requires:
- `lcov` - Coverage tool
- `genhtml` - HTML report generator

Install on macOS:
```bash
brew install lcov
```

Install on Ubuntu/Debian:
```bash
sudo apt-get install lcov
```

### Coverage Goals

The project targets:
- **>85% overall code coverage**
- **100% coverage on critical paths** (routing, agent movement, metrics)
- Comprehensive coverage of all design patterns
- Full coverage of UI component logic

## Video Walkthrough

[View Video](https://youtu.be/sEiNyzS8kTU)

Demonstrates: Building the project, running CLI demo, comparing routing policies, design pattern implementation, and architecture overview.

