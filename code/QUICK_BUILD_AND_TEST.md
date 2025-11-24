# Quick Build and Test Guide

## The Problem

The test executable `test_creational_patterns` doesn't exist because the project hasn't been built yet.

## Solution: Build with Qt Creator

Due to the Qt6 CMake recursion bug, you need to build using Qt Creator instead of command-line CMake.

### Step 1: Open Qt Creator

```bash
open -a "Qt Creator"
```

Or find it in Applications.

### Step 2: Open the Project

1. In Qt Creator: **File → Open File or Project**
2. Navigate to: `/Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/CMakeLists.txt`
3. Click **Open**

### Step 3: Configure Project

1. Qt Creator will ask you to configure the project
2. Select a **Kit** (should auto-detect Qt 6.9.3)
3. Click **Configure Project**

### Step 4: Build

1. Click **Build → Build Project** (or press `Cmd+B`)
2. Wait for compilation to complete
3. Check the **Compile Output** panel for any errors

### Step 5: Run Tests

After building, the test executable will be in the build directory:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
./test_creational_patterns
```

## Alternative: Check if Already Built

If you've built before, check what's available:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ls -la test_*
```

If you see test executables, you can run them directly.

## What Gets Built

After successful build, you'll have:
- `build/test_creational_patterns` - Creational patterns tests (27 tests)
- `build/test_city` - City tests
- `build/test_grid_view` - GridView tests
- `build/test_main_window` - MainWindow tests
- `build/test_metrics_panel` - MetricsPanel tests
- `build/test_modern_ui` - Modern UI tests
- And all other test executables...

## Run All Tests

After building, you can run all tests:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

This will run all C++ tests and Python verification scripts.

## Troubleshooting

### Qt Creator not found?
```bash
brew install --cask qt-creator
```

### Build fails?
- Check **Compile Output** panel in Qt Creator
- Make sure Qt 6.9.3 is selected in the kit
- Try **Build → Clean All**, then rebuild

### Still having issues?
The CMake recursion bug prevents command-line builds. Qt Creator is the recommended solution.

