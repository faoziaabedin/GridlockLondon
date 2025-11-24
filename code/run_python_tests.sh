#!/bin/bash

# Run Python tests only (these don't require building)
# Use this when C++ tests can't be built due to Qt CMake issues

echo "=========================================="
echo "  GridlockLondon - Python Tests Only"
echo "=========================================="
echo ""
echo "Note: C++ tests require building first."
echo "Use Qt Creator to build, then run ./run_all_tests.sh"
echo ""
echo "=========================================="
echo ""

cd "$(dirname "$0")"

PASSED=0
FAILED=0
TOTAL=0

run_python_test() {
    local test_name=$1
    local script_path=$2
    
    TOTAL=$((TOTAL + 1))
    echo "[$TOTAL] Running $test_name..."
    
    if [ ! -f "$script_path" ]; then
        echo "    ✗ FAILED: $script_path not found"
        FAILED=$((FAILED + 1))
        return 1
    fi
    
    if python3 "$script_path" > /tmp/test_output_$$.txt 2>&1; then
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

echo "=== Python Verification Scripts ==="
echo ""

run_python_test "verify_tests.py" "verify_tests.py"
run_python_test "verify_route_planner.py" "verify_route_planner.py"
run_python_test "verify_congestion_policy.py" "verify_congestion_policy.py"
run_python_test "final_verification.py" "final_verification.py"

echo ""
echo "=========================================="
echo "  Test Summary"
echo "=========================================="
echo "Total Tests: $TOTAL"
echo "Passed: $PASSED"
echo "Failed: $FAILED"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "🎉 ALL PYTHON TESTS PASSED! 🎉"
    echo ""
    echo "To run C++ tests, build the project first:"
    echo "  1. Open Qt Creator"
    echo "  2. File → Open File or Project"
    echo "  3. Select: code/CMakeLists.txt"
    echo "  4. Build → Build Project"
    echo "  5. Then run: ./run_all_tests.sh"
    exit 0
else
    echo "❌ Some tests failed. Please review the output above."
    exit 1
fi

