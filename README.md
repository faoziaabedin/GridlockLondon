[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/8Cy36LS2)
[![Work in MakeCode](https://classroom.github.com/assets/work-in-make-code-8824cc13a1a3f34ffcd245c82f0ae96fdae6b7d554b6539aec3a03a70825519c.svg)](https://classroom.github.com/online_ide?assignment_repo_id=21096241&assignment_repo_type=AssignmentRepo)
# Assignment 2 – Low-Fidelity Prototype

# GridlockLondon - Traffic Simulation System

## Project Overview

GridlockLondon simulates traffic flow on a grid network where agents (vehicles) navigate from origin to destination using different routing policies. The system models road capacity constraints, dynamic congestion, and provides real-time metrics for traffic analysis.

**Deliverable:** D2 - Intermediate Design & Partial Implementation (30%)  
**Due Date:** November 09, 2025

## Prerequisites

- **C++20 compatible compiler** (GCC 10+, Clang 11+, or MSVC 2019+)
- **CMake** 3.16 or higher
- **Qt 6.x** (optional for D2)
- **Git**

## Build Instructions

### Clone the Repository
```bash
git clone <your-repository-url>
cd GridlockLondon
```

### Build with CMake
```bash
cd code
mkdir build
cd build
cmake ..
make
```

### Run the CLI Demo
```bash
./gridlock_demo
# Or with custom preset:
./gridlock_demo ../presets/london_10x10.json
```

### Alternative: Qt Creator
1. Open Qt Creator
2. File → Open File or Project → Select `code/CMakeLists.txt`
3. Build → Build Project
4. Run from build directory

## Project Structure

```
GridlockLondon/
├── code/
│   ├── CMakeLists.txt           # Build configuration
│   ├── main.cpp                 # CLI demo entry point
│   ├── core/                    # Domain logic
│   │   ├── City.h/.cpp          # Graph model + occupancy
│   │   ├── Agent.h/.cpp         # Mobile vehicle entity
│   │   ├── IRoutePolicy.h       # Strategy interface
│   │   ├── ShortestPathPolicy.h/.cpp
│   │   ├── CongestionAwarePolicy.h/.cpp
│   │   ├── RoutePlanner.h/.cpp  # Dijkstra pathfinding
│   │   ├── Metrics.h/.cpp       # KPI tracking
│   │   └── SimulationController.h/.cpp
│   ├── adapters/                # I/O and persistence
│   │   ├── PresetLoader.h/.cpp  # JSON configuration loading
│   │   └── TimerService.h/.cpp
│   ├── ui/                      # Presentation (stubbed for D2)
│   └── presets/                 # JSON scenario configs
└── report/
    ├── Design_Rationale.pdf
    ├── Video_Walkthrough.mp4
    └── uml/                     # Class, use case, sequence diagrams
```

## Design Overview

### Architecture
The system uses **layered architecture** with three main layers:
- **Core**: Pure domain logic (graph model, agents, routing, metrics)
- **Adapters**: External I/O (JSON loading, timers, export)
- **UI**: Presentation layer (stubbed for D2, full implementation in D3)

### Design Patterns

**Strategy Pattern** (`IRoutePolicy`, `ShortestPathPolicy`, `CongestionAwarePolicy`)  
Allows runtime selection between routing algorithms.

**Facade Pattern** (`RoutePlanner`)  
Simplifies complex pathfinding operations behind a clean interface.

**Observer Pattern** (`Metrics`)  
Tracks simulation events without tight coupling.

## How Design Maps to Diagrams

********** ADDDDDD *****************

## Team Member Contributions

### Faozia 
- Core domain classes: City, Node, Edge (Issue #1)
- Preset configuration class (Issue #2)
- SimulationController orchestrator (Issue #3)
- CLI demo runner (Issue #4)
- Agent movement logic (Issue #5)

### Paridhi
- Routing policy interface and ShortestPathPolicy (Issue #5)
- CongestionAwarePolicy implementation (Issue #6)
- RoutePlanner with Dijkstra algorithm (Issue #7)
- Metrics tracking system (Issue #8)
- PresetLoader JSON parsing (Issue #9)
- Adapter implementations (Issue #10)

### Collaborative Work
- UML diagrams (class, use case, sequence)
- Design rationale document
- Video walkthrough (3-5 minutes)
- Code reviews and integration testing

## Video Walkthrough

[View 3-5 minute demonstration](report/Video_Walkthrough.mp4)

Demonstrates: Building the project, running CLI demo, comparing routing policies, design pattern implementation, and architecture overview.

## Configuration

Create custom JSON presets in `code/presets/`:

```json
{
  "name": "london_10x10",
  "rows": 10,
  "cols": 10,
  "blocked": [[12, 13], [45, 55]],
  "agentCount": 50,
  "tickMs": 150,
  "policy": "CONGESTION_AWARE"
}
```

## Additional Resources

- **Design Rationale**: `report/Design_Rationale.pdf`
- **UML Diagrams**: `report/uml/`
- **Technical Specification**: Project documentation folder

---
