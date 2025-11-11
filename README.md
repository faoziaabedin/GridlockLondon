[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/8Cy36LS2)
[![Work in MakeCode](https://classroom.github.com/assets/work-in-make-code-8824cc13a1a3f34ffcd245c82f0ae96fdae6b7d554b6539aec3a03a70825519c.svg)](https://classroom.github.com/online_ide?assignment_repo_id=21096241&assignment_repo_type=AssignmentRepo)
# Deliverable 2 – Low-Fidelity Prototype

# GridlockLondon - Traffic Simulation System

**Authors:**  
- Faozia Abedin (251358251, fabedin4@uwo.ca)  
- Paridhi Mehla (251371402, pmehla2@uwo.ca)


## Project Overview

GridlockLondon simulates traffic flow on a grid network where agents (vehicles) navigate from origin to destination using different routing policies. The system models road capacity constraints, dynamic congestion, and provides real-time metrics for traffic analysis.

**Deliverable:** D2 - Intermediate Design & Partial Implementation  
**Due Date:** November 09, 2025


## Build Instructions

### Clone the Repository
```bash
git clone <repository-url>
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


## Video Walkthrough

[View Video](https://youtu.be/sEiNyzS8kTU)

Demonstrates: Building the project, running CLI demo, comparing routing policies, design pattern implementation, and architecture overview.

