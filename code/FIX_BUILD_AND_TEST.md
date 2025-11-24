# Fix: Test Executables Not Found

## Problem
When running `./run_all_tests.sh`, you get:
```
✗ FAILED: test_city not found
```

This means the test executables haven't been built yet.

## Solution Options

### Option 1: Use Qt Creator to Build (Recommended)

1. **Open Qt Creator**
2. **File → Open File or Project**
3. **Navigate to:** `/Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/CMakeLists.txt`
4. **Click "Configure Project"**
5. **Build → Build Project** (Ctrl+B / Cmd+B)
6. **Wait for build to complete**
7. **Then run tests:**
   ```bash
   cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
   ./run_all_tests.sh
   ```

### Option 2: Try Command Line Build

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
rm -rf CMakeCache.txt CMakeFiles
cmake ..
make -j4
cd ..
./run_all_tests.sh
```

**Note:** This may fail with Qt CMake recursion error. If it does, use Option 1.

### Option 3: Run Python Tests Only (Always Works)

These tests don't require building:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
python3 verify_tests.py
python3 verify_route_planner.py
python3 verify_congestion_policy.py
python3 final_verification.py
```

### Option 4: Check if Tests Exist Elsewhere

```bash
# Find any existing test executables
find /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi -name "test_*" -type f -executable 2>/dev/null

# If found, run them directly
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
./test_city
./test_grid_view
# etc.
```

## Quick Check Commands

```bash
# Check if build directory has anything
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ls -la

# Check if Makefile exists
test -f Makefile && echo "Can try: make" || echo "Need: cmake .."

# Check if any test executables exist
ls test_* 2>/dev/null || echo "No test executables found"
```

## After Building Successfully

Once tests are built, you can run:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

This will run:
- All C++ unit tests (Qt Test + GoogleTest)
- All Python verification scripts
- Show summary at the end

