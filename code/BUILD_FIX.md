# Fix for Qt6 CMake Recursion Error

## The Problem
Qt6 CMake configuration has a recursion bug causing "Maximum recursion depth exceeded"

## Solution Options:

### Option 1: Update Qt6 (Recommended)
```bash
brew upgrade qt@6
cd code/build
rm -rf *
cmake ..
make
```

### Option 2: Use Qt Creator Instead
1. Open Qt Creator
2. File → Open File or Project
3. Select `code/CMakeLists.txt`
4. Click "Configure Project"
5. Build → Build Project

### Option 3: Temporary Workaround - Build Core Only
If you just need to test core functionality:
```bash
cd code/build
# Create a minimal CMakeLists.txt that doesn't use Qt
# Then build just the core library
```

### Option 4: Check Qt Version
```bash
brew list qt@6
qmake6 --version
```

If Qt is outdated, update it:
```bash
brew upgrade qt@6
```

## Quick Test (if executables exist)
If test executables were built before deletion:
```bash
cd code/build
ls test_*  # Check if they exist
./test_city  # Run directly
```

## After Fixing Qt
```bash
cd code/build
rm -rf *
cmake ..
make -j4
cd ..
./run_all_tests.sh
```
