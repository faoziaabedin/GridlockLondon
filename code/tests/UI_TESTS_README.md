# UI Tests Documentation

This directory contains comprehensive tests for the GridlockLondon UI components.

## Test Files

### 1. `test_grid_view.cpp`
Qt Test framework tests for the GridView component.

**Tests:**
- GridView creation and initialization
- Simulation controller connection
- Scene updates
- Node items creation
- Edge items creation
- Agent items creation
- Zoom functionality (1x to 5x)
- Grid overlay toggle
- Node labels toggle
- Fit to window
- Agent selection
- Animation updates

**Run:**
```bash
cd build
./test_grid_view
```

### 2. `test_main_window.cpp`
Qt Test framework tests for the MainWindow component.

**Tests:**
- MainWindow creation
- UI component existence
- Control buttons (Start, Pause, Reset)
- View controls (Fit to Window, Grid toggle, Node labels toggle)
- Simulation controller integration
- Agent selection handling
- Metrics updates

**Run:**
```bash
cd build
./test_main_window
```

### 3. `test_metrics_panel.cpp`
Qt Test framework tests for the MetricsPanel component.

**Tests:**
- MetricsPanel creation
- Simulation controller connection
- Metrics updates
- Label display

**Run:**
```bash
cd build
./test_metrics_panel
```

### 4. `test_ui_integration.cpp`
Integration test for all UI components working together.

**Tests:**
- Full UI workflow
- Simulation controller integration
- Component interaction
- Simulation run/pause/reset
- UI updates during simulation

**Run:**
```bash
cd build
./test_ui_integration
```

### 5. `test_ui_graphics_items.cpp`
Functional test for graphics items and GridView features.

**Tests:**
- Graphics items creation
- Zoom functionality
- Grid overlay
- Node labels
- Fit to window
- Animation
- Agent selection

**Run:**
```bash
cd build
./test_ui_graphics_items
```

## Building Tests

All UI tests require Qt6 and are automatically built when you build the project:

```bash
cd code
mkdir build
cd build
cmake ..
make
```

The test executables will be created in the `build` directory.

## Running All Tests

You can run all tests using the test script:

```bash
cd code
./run_tests.sh
```

Or run individual tests:

```bash
cd build
./test_grid_view
./test_main_window
./test_metrics_panel
./test_ui_integration
./test_ui_graphics_items
```

## Test Coverage

### GridView
- ✅ Basic creation and initialization
- ✅ Scene updates
- ✅ Node/Edge/Agent rendering
- ✅ Zoom controls (1x-5x)
- ✅ Pan functionality
- ✅ Fit to window
- ✅ Grid overlay toggle
- ✅ Node labels toggle
- ✅ Edge color updates
- ✅ Agent position updates
- ✅ Agent selection

### MainWindow
- ✅ UI setup
- ✅ Control buttons
- ✅ View controls
- ✅ Simulation integration
- ✅ Agent info panel

### MetricsPanel
- ✅ Metrics display
- ✅ Real-time updates
- ✅ Simulation integration

## Notes

- All UI tests require a QApplication instance (automatically created)
- Tests use the Qt Test framework for structured testing
- Integration tests verify end-to-end functionality
- Graphics items tests verify visual component behavior

## Requirements

- Qt6 (Widgets, Core, Test components)
- CMake 3.10+
- C++20 compiler

