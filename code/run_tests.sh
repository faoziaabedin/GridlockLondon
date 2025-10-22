#!/bin/bash

# Simple test runner for route policy tests
echo "=== Route Policy Test Runner ==="
echo

# Set up compilation flags
CXX="g++"
CXXFLAGS="-std=c++20 -Wall -Wextra -I."
SOURCES="core/Node.cpp core/Edge.cpp core/City.cpp core/Agent.cpp core/Preset.cpp core/ShortestPathPolicy.cpp"

# Test 1: Requirements Verification Test
echo "Test 1: Requirements Verification..."
echo "Compiling test_requirements_verification..."
if $CXX $CXXFLAGS -o test_requirements_verification $SOURCES tests/test_requirements_verification.cpp 2>/dev/null; then
    echo "✓ Compilation successful"
    echo "Running test_requirements_verification..."
    if ./test_requirements_verification; then
        echo "✓ test_requirements_verification PASSED"
    else
        echo "✗ test_requirements_verification FAILED"
    fi
    rm -f test_requirements_verification
else
    echo "✗ Compilation failed for test_requirements_verification"
fi
echo

# Test 2: Route Policy Contract Test
echo "Test 2: Route Policy Contract..."
echo "Compiling test_route_policy_contract..."
if $CXX $CXXFLAGS -o test_route_policy_contract $SOURCES tests/test_route_policy_contract.cpp 2>/dev/null; then
    echo "✓ Compilation successful"
    echo "Running test_route_policy_contract..."
    if ./test_route_policy_contract; then
        echo "✓ test_route_policy_contract PASSED"
    else
        echo "✗ test_route_policy_contract FAILED"
    fi
    rm -f test_route_policy_contract
else
    echo "✗ Compilation failed for test_route_policy_contract"
fi
echo

# Test 3: Comprehensive Route Policy Test
echo "Test 3: Comprehensive Route Policy..."
echo "Compiling test_route_policy..."
if $CXX $CXXFLAGS -o test_route_policy $SOURCES tests/test_route_policy.cpp 2>/dev/null; then
    echo "✓ Compilation successful"
    echo "Running test_route_policy..."
    if ./test_route_policy; then
        echo "✓ test_route_policy PASSED"
    else
        echo "✗ test_route_policy FAILED"
    fi
    rm -f test_route_policy
else
    echo "✗ Compilation failed for test_route_policy"
fi
echo

# Test 4: Simple Integration Test
echo "Test 4: Simple Integration..."
echo "Compiling test_route_policy_simple..."
if $CXX $CXXFLAGS -o test_route_policy_simple $SOURCES test_route_policy_simple.cpp 2>/dev/null; then
    echo "✓ Compilation successful"
    echo "Running test_route_policy_simple..."
    if ./test_route_policy_simple; then
        echo "✓ test_route_policy_simple PASSED"
    else
        echo "✗ test_route_policy_simple FAILED"
    fi
    rm -f test_route_policy_simple
else
    echo "✗ Compilation failed for test_route_policy_simple"
fi
echo

echo "=== Test Runner Complete ==="
