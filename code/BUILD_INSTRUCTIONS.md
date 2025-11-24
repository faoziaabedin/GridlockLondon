# Build Instructions - Qt6 CMake Issue Fix

## Current Issue
Qt6 6.9.3 has a CMake recursion bug that prevents configuration.

## Immediate Solutions

### Solution 1: Run Python Tests (Works Now)
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
python3 verify_tests.py
python3 verify_route_planner.py
python3 verify_congestion_policy.py
python3 final_verification.py
```

### Solution 2: Try Qt Creator
1. Open Qt Creator
2. File → Open File or Project
3. Navigate to: `/Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/CMakeLists.txt`
4. Click "Configure Project"
5. Build → Build Project (Ctrl+B / Cmd+B)

### Solution 3: Downgrade Qt (if needed)
```bash
brew uninstall qt@6
brew install qt@6@6.8
```

### Solution 4: Wait for Qt Fix
This is a known Qt6.9.3 bug. You can:
- Check Qt bug tracker for updates
- Use Qt Creator as workaround
- Build manually without CMake (complex)

## What Works Right Now
- ✅ Python verification scripts
- ✅ Git repository (all code is safe)
- ❌ CMake build (Qt recursion bug)

## Recommended Action
**Use Qt Creator to build** - it handles Qt CMake issues better than command line.

## After Building
Once you have test executables:
```bash
cd code
./run_all_tests.sh
```

