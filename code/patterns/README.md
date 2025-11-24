# Creational Design Patterns Implementation

This directory contains the implementation of three creational design patterns for the GridlockLondon traffic simulation project.

## Patterns Implemented

### 1. Factory Pattern (`IGridFactory.h`, `RegularGridFactory.*`, `RandomGridFactory.*`, `RealWorldGridFactory.*`)

**Purpose:** Create different grid topologies without coupling client code to specific implementations.

**Files:**
- `IGridFactory.h` - Abstract factory interface
- `RegularGridFactory.h/cpp` - Creates standard rectangular grids
- `RandomGridFactory.h/cpp` - Creates grids with random connections
- `RealWorldGridFactory.h/cpp` - Creates highway-style networks

**Usage:**
```cpp
RegularGridFactory factory;
auto city = factory.createGrid(5, 5);
```

### 2. Builder Pattern (`PresetBuilder.h/cpp`)

**Purpose:** Construct complex `Preset` objects with a fluent interface.

**Files:**
- `PresetBuilder.h/cpp` - Builder class with fluent interface

**Usage:**
```cpp
Preset preset = PresetBuilder()
    .setName("Rush Hour")
    .setGridSize(10, 10)
    .setAgentCount(50)
    .setPolicy(PolicyType::CONGESTION_AWARE)
    .build();
```

### 3. Abstract Factory Pattern (`IPolicyFactory.h`, `ShortestPathFactory.*`, `CongestionAwareFactory.*`, `PolicyRegistry.*`)

**Purpose:** Create families of routing policies with extensibility and discoverability.

**Files:**
- `IPolicyFactory.h` - Abstract factory interface
- `ShortestPathFactory.h/cpp` - Factory for ShortestPath policy
- `CongestionAwareFactory.h/cpp` - Factory for CongestionAware policy
- `PolicyRegistry.h/cpp` - Registry for dynamic policy selection

**Usage:**
```cpp
PolicyRegistry& registry = PolicyRegistry::getInstance();
auto policy = registry.createPolicy("CongestionAware");
```

## Testing

Run the comprehensive test suite:
```bash
cd build
./test_creational_patterns
```

The test suite includes:
- Factory Pattern tests (polymorphism, different grid types)
- Builder Pattern tests (fluent interface, validation, reset)
- Abstract Factory Pattern tests (registry, extensibility)

## Documentation

See `DESIGN_PATTERNS_DOCUMENTATION.md` for:
- Detailed explanations of each pattern
- UML diagrams
- Problem statements and solutions
- Interview talking points
- Usage examples

## Integration

These patterns are integrated into the main codebase:
- `PresetLoader` can use `IGridFactory` implementations
- `SimulationController` can use `PresetBuilder` for preset creation
- `SimulationController` can use `PolicyRegistry` for policy creation

## Design Principles Demonstrated

- **Open/Closed Principle**: Extend without modifying existing code
- **Dependency Inversion**: Depend on abstractions, not concretions
- **Single Responsibility**: Each factory creates one type of object
- **Polymorphism**: Use interfaces for flexibility

## Future Extensions

- Add more grid factories (HexagonalGridFactory, CircularGridFactory)
- Add more policy factories (PredictivePolicyFactory, AdaptivePolicyFactory)
- Load factories from plugins or configuration files
- Use Builder to construct presets from JSON/XML

