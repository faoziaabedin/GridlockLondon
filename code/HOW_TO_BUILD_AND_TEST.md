# How to Build and Test - Complete Guide

## Current Situation

✅ **Python tests work** - They verify core logic without needing a build  
❌ **C++ tests need building** - Test executables don't exist yet  
⚠️ **Qt CMake bug** - Prevents command-line builds (known Qt6.9.3 issue)

## Solution: Build with Qt Creator

### Step-by-Step Instructions

1. **Open Qt Creator**
   - Launch Qt Creator application

2. **Open the Project**
   - File → Open File or Project
   - Navigate to: `/Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/CMakeLists.txt`
   - Click "Open"

3. **Configure Project**
   - Qt Creator will ask to configure the project
   - Click "Configure Project"
   - Wait for configuration to complete

4. **Build the Project**
   - Build → Build Project (or press ⌘B / Ctrl+B)
   - Wait for build to complete (may take a few minutes)
   - Check the "Compile Output" pane for any errors

5. **Verify Build Success**
   - Look for "Build finished" message
   - Check that test executables exist:
     ```bash
     cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
     ls test_*
     ```

6. **Run All Tests**
   ```bash
   cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
   ./run_all_tests.sh
   ```

## What Works Right Now (No Build Needed)

### Run Python Tests
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_python_tests.sh
```

Or individually:
```bash
python3 verify_tests.py
python3 verify_route_planner.py
python3 verify_congestion_policy.py
python3 final_verification.py
```

## Quick Commands Reference

### If Tests Are Already Built
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

### If Tests Need Building
1. Build with Qt Creator (see instructions above)
2. Then run: `./run_all_tests.sh`

### Python Tests Only (Always Works)
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_python_tests.sh
```

## What Gets Tested

### Python Tests (No Build Required)
- ✅ Core logic verification
- ✅ Route planner functionality
- ✅ Congestion policy verification
- ✅ Final requirements check

### C++ Tests (Require Build)
- ✅ City and grid functionality
- ✅ UI components (GridView, MainWindow, MetricsPanel)
- ✅ Modern UI features
- ✅ Issue 4 features (PresetGallery, AgentInspector, ComparativeAnalysis)
- ✅ GoogleTest suites (RoutePlanner, Agent, Metrics, SimulationController, Factory Pattern)

## Troubleshooting

### "test_city not found"
**Solution:** Build the project first using Qt Creator

### Qt CMake Recursion Error
**Solution:** Use Qt Creator instead of command-line cmake

### Build Succeeds but Tests Still Not Found
**Solution:** Check that executables are in `code/build/` directory:
```bash
cd code/build
ls test_*
```

### Want to Skip C++ Tests
**Solution:** Run Python tests only:
```bash
./run_python_tests.sh
```

## Expected Output After Building

When you run `./run_all_tests.sh` after building, you should see:

```
==========================================
  GridlockLondon - Complete Test Suite
==========================================

=== C++ Unit Tests ===

[1] Running test_city...
    ✓ PASSED
[2] Running test_grid_view...
    ✓ PASSED
...
[19] Running Factory Pattern Test Suite...
    ✓ PASSED

=== Python Verification Scripts ===

[20] Running verify_tests.py...
    ✓ PASSED
...

==========================================
  Test Summary
==========================================
Total Tests: 19
Passed: 19
Failed: 0

🎉 ALL TESTS PASSED! 🎉
```

