#!/bin/bash

# Comprehensive test runner for all assignment tests
# This script runs all tests and verification scripts

set -e  # Exit on error

echo "=========================================="
echo "  GridlockLondon - Complete Test Suite"
echo "=========================================="
echo ""

cd "$(dirname "$0")/build" || exit 1

PASSED=0
FAILED=0
TOTAL=0

# Function to run a test and track results
run_test() {
    local test_name=$1
    local test_executable=$2
    
    TOTAL=$((TOTAL + 1))
    echo "[$TOTAL] Running $test_name..."
    
    if [ ! -f "$test_executable" ]; then
        echo "    ✗ FAILED: $test_executable not found"
        FAILED=$((FAILED + 1))
        return 1
    fi
    
    ./"$test_executable" > /tmp/test_output_$$.txt 2>&1
    local exit_code=$?
    
    # Check test output for actual failures
    # For Qt tests, look for "Totals: X passed, Y failed" pattern
    if grep -q "Totals:" /tmp/test_output_$$.txt; then
        # Extract failure count from "Totals: X passed, Y failed"
        failed_count=$(grep "Totals:" /tmp/test_output_$$.txt | sed -n 's/.*\([0-9]\+\) failed.*/\1/p' | head -1)
        if [ -z "$failed_count" ] || [ "$failed_count" = "0" ]; then
            echo "    ✓ PASSED"
            PASSED=$((PASSED + 1))
            return 0
        else
            echo "    ✗ FAILED: $failed_count test(s) failed"
            echo "    Last 5 lines of output:"
            tail -5 /tmp/test_output_$$.txt | sed 's/^/      /'
            FAILED=$((FAILED + 1))
            return 1
        fi
    # For non-Qt tests, look for success patterns
    elif grep -qiE "All.*Pass|SUCCESS|Completed Successfully" /tmp/test_output_$$.txt; then
        echo "    ✓ PASSED"
        PASSED=$((PASSED + 1))
        return 0
    # If exit code is 0 and no failure indicators, assume success
    elif [ $exit_code -eq 0 ]; then
        echo "    ✓ PASSED"
        PASSED=$((PASSED + 1))
        return 0
    else
        echo "    ✗ FAILED: Exit code $exit_code"
        echo "    Last 5 lines of output:"
        tail -5 /tmp/test_output_$$.txt | sed 's/^/      /'
        FAILED=$((FAILED + 1))
        return 1
    fi
}

# Function to run a Python verification script
run_python_verification() {
    local test_name=$1
    local script_path=$2
    
    TOTAL=$((TOTAL + 1))
    echo "[$TOTAL] Running $test_name..."
    
    if [ ! -f "../$script_path" ]; then
        echo "    ✗ FAILED: $script_path not found"
        FAILED=$((FAILED + 1))
        return 1
    fi
    
    if cd .. && python3 "$script_path" > /tmp/test_output_$$.txt 2>&1 && cd build; then
        if grep -q "✓\|✅\|PASS\|SUCCESS\|All.*Pass" /tmp/test_output_$$.txt; then
            echo "    ✓ PASSED"
            PASSED=$((PASSED + 1))
            return 0
        else
            echo "    ✗ FAILED: Verification script did not report success"
            echo "    Last 5 lines of output:"
            tail -5 /tmp/test_output_$$.txt | sed 's/^/      /'
            FAILED=$((FAILED + 1))
            return 1
        fi
    else
        echo "    ✗ FAILED"
        echo "    Last 5 lines of output:"
        tail -5 /tmp/test_output_$$.txt | sed 's/^/      /'
        FAILED=$((FAILED + 1))
        return 1
    fi
}

echo "=== C++ Unit Tests ==="
echo ""

# Core tests
run_test "test_city" "test_city"

# UI tests
run_test "test_grid_view" "test_grid_view"
run_test "test_main_window" "test_main_window"
run_test "test_metrics_panel" "test_metrics_panel"
run_test "test_ui_graphics_items" "test_ui_graphics_items"
run_test "test_ui_integration" "test_ui_integration"
run_test "test_modern_ui" "test_modern_ui"

echo ""
echo "=== Python Verification Scripts ==="
echo ""

# Python verification scripts
run_python_verification "verify_tests.py" "verify_tests.py"
run_python_verification "verify_route_planner.py" "verify_route_planner.py"
run_python_verification "verify_congestion_policy.py" "verify_congestion_policy.py"
run_python_verification "final_verification.py" "final_verification.py"

echo ""
echo "=========================================="
echo "  Test Summary"
echo "=========================================="
echo "Total Tests: $TOTAL"
echo "Passed: $PASSED"
echo "Failed: $FAILED"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "🎉 ALL TESTS PASSED! 🎉"
    exit 0
else
    echo "❌ Some tests failed. Please review the output above."
    exit 1
fi

