# Commands to Run All Tests

## Quick Command (All-in-One)

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

## Step-by-Step Commands

### Option 1: If Build Directory Exists (Tests Already Built)

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

### Option 2: Build First, Then Test

```bash
# Navigate to code directory
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code

# Try to build (may fail due to Qt CMake issue)
cd build
cmake ..
make -j4

# If build succeeds, run tests
cd ..
./run_all_tests.sh
```

### Option 3: Use Qt Creator to Build

1. Open Qt Creator
2. File → Open File or Project
3. Select: `code/CMakeLists.txt`
4. Click "Configure Project"
5. Build → Build Project (Ctrl+B / Cmd+B)
6. Then run tests:
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./run_all_tests.sh
```

## Individual Test Commands

### Run All C++ Tests
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ctest
```

### Run GoogleTest Suites Only
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
./test_route_planner_googletest
./test_agent_googletest
./test_metrics_googletest
./test_simulation_controller_googletest
./test_factory_pattern_googletest
```

### Run Python Verification
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
python3 verify_tests.py
python3 verify_route_planner.py
python3 verify_congestion_policy.py
python3 final_verification.py
```

## Check Test Results

After running `./run_all_tests.sh`, you'll see:
- ✓ PASSED for each test
- Summary at the end
- Total tests, passed, failed counts

## If Build Fails (Qt CMake Issue)

The Python tests will still work:
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
python3 verify_tests.py
python3 final_verification.py
```

