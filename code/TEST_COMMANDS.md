# Test Commands - Quick Reference

## Prerequisites

**IMPORTANT**: Build the project first using Qt Creator (due to Qt6 CMake recursion bug).

```bash
# 1. Open Qt Creator
open -a "Qt Creator"

# 2. In Qt Creator:
#    - File → Open File or Project
#    - Select: code/CMakeLists.txt
#    - Configure Project (select Qt 6.9.3 kit)
#    - Build → Build Project (Cmd+B / Ctrl+B)
```

## Quick Test Commands

### Run All Tests (Recommended)

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

This runs:
- All C++ unit tests
- All Python verification scripts
- Shows summary at the end

### Run Python Tests Only (No Build Required)

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_python_tests.sh
```

## Individual Test Suites

### UI/UX Polish Tests

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
./test_ui_polish
```

**Expected**: 29 tests pass

### Creational Patterns Tests

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
./test_creational_patterns
```

**Expected**: 27 tests pass

### Core Component Tests

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build

# City tests
./test_city

# GridView tests
./test_grid_view

# MainWindow tests
./test_main_window

# MetricsPanel tests
./test_metrics_panel

# Modern UI tests
./test_modern_ui
```

### GoogleTest Suites

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build

# RoutePlanner tests
./test_route_planner_googletest

# Agent tests
./test_agent_googletest

# Metrics tests
./test_metrics_googletest

# SimulationController tests
./test_simulation_controller_googletest

# Factory Pattern tests
./test_factory_pattern_googletest
```

### Run All GoogleTest Suites via CTest

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ctest --output-on-failure
```

## Python Verification Scripts

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code

# Individual scripts
python3 verify_tests.py
python3 verify_route_planner.py
python3 verify_congestion_policy.py
python3 final_verification.py
```

## Code Coverage

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_coverage.sh
```

**Output**: `build/html_coverage/index.html`

## Check What Tests Are Available

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ls -la test_*
```

## One-Liner: Build and Test Everything

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code && \
echo "⚠️  Build with Qt Creator first, then run:" && \
echo "   ./run_all_tests.sh"
```

## Troubleshooting

### "test_* not found"
**Solution**: Build the project first using Qt Creator

### "Permission denied"
**Solution**: 
```bash
chmod +x run_all_tests.sh
chmod +x run_python_tests.sh
chmod +x run_coverage.sh
```

### Tests fail to compile
**Solution**: Check that all source files are in `CMakeLists.txt`

## Expected Test Counts

- **UI Polish Tests**: 29 tests
- **Creational Patterns Tests**: 27 tests
- **Core Component Tests**: 50+ tests
- **GoogleTest Suites**: 50+ tests
- **Python Verification**: 4 scripts

**Total**: 150+ test cases

## Quick Status Check

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build

# Check if tests exist
if [ -f test_ui_polish ]; then
    echo "✅ UI Polish tests ready"
    ./test_ui_polish | tail -3
else
    echo "❌ Build required (use Qt Creator)"
fi

if [ -f test_creational_patterns ]; then
    echo "✅ Creational Patterns tests ready"
    ./test_creational_patterns | tail -3
else
    echo "❌ Build required (use Qt Creator)"
fi
```

