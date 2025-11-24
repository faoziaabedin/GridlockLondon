# Quick Fix for Build Issues

## The Problem
You accidentally ran `rm -rf *` from the `code` directory, which deleted all files.

## Solution
Files have been restored from git. However, Issue 4 files (PresetGallery, AgentInspector, ComparativeAnalysis) were never committed to git, so they're missing.

## To Build and Test:

### Option 1: Build from root directory (recommended)
```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi
mkdir -p build
cd build
cmake ..
make
```

### Option 2: Build from code directory
```bash
cd code
mkdir -p build
cd build
cmake ..
make
```

### Run Tests
```bash
cd code
./run_all_tests.sh
```

### Run Application
```bash
cd code/build
./gridlock_ui
```

## Note
If you get Qt CMake errors, try:
1. Update Qt6: `brew upgrade qt@6`
2. Or use the root-level build directory instead
