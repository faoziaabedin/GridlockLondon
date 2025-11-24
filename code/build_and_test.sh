#!/bin/bash

# Comprehensive build and test script
# Handles building and running all tests

set -e

echo "=========================================="
echo "  GridlockLondon - Build & Test"
echo "=========================================="
echo ""

cd "$(dirname "$0")"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

cd build

# Try to configure and build
echo "Attempting to configure CMake..."
if cmake .. 2>&1 | tee /tmp/cmake_output.txt; then
    echo ""
    echo "✓ CMake configuration successful"
    echo ""
    echo "Building project..."
    if make -j4 2>&1 | tee /tmp/build_output.txt; then
        echo ""
        echo "✓ Build successful"
        echo ""
        cd ..
        echo "Running all tests..."
        ./run_all_tests.sh
    else
        echo ""
        echo "✗ Build failed"
        echo "Last 10 lines of build output:"
        tail -10 /tmp/build_output.txt
        echo ""
        echo "Note: If you see Qt CMake recursion errors, use Qt Creator to build instead."
        echo ""
        echo "Running Python tests (these don't require building):"
        cd ..
        python3 verify_tests.py
        python3 verify_route_planner.py
        python3 verify_congestion_policy.py
        python3 final_verification.py
        exit 1
    fi
else
    echo ""
    echo "✗ CMake configuration failed"
    echo "Last 10 lines of CMake output:"
    tail -10 /tmp/cmake_output.txt
    echo ""
    echo "Note: This is likely the Qt CMake recursion bug."
    echo "Workaround: Use Qt Creator to build the project."
    echo ""
    echo "Running Python tests (these don't require building):"
    cd ..
    python3 verify_tests.py
    python3 verify_route_planner.py
    python3 verify_congestion_policy.py
    python3 final_verification.py
    exit 1
fi

