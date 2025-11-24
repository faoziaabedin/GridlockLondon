#!/bin/bash

# Script to build with coverage and generate coverage report
# Usage: ./run_coverage.sh

set -e

echo "=========================================="
echo "  GridlockLondon - Coverage Report"
echo "=========================================="
echo ""

cd "$(dirname "$0")/build" || exit 1

# Clean previous coverage data
echo "Cleaning previous coverage data..."
find . -name "*.gcda" -delete
find . -name "*.gcno" -delete
rm -rf coverage_html coverage.info

# Configure with coverage enabled
echo "Configuring CMake with coverage enabled..."
cmake .. -DENABLE_COVERAGE=ON

# Build
echo "Building project..."
make -j4

# Run tests
echo "Running tests..."
ctest --output-on-failure

# Check if lcov is available
if ! command -v lcov &> /dev/null; then
    echo "ERROR: lcov is not installed."
    echo "Install with: brew install lcov (macOS) or apt-get install lcov (Linux)"
    exit 1
fi

if ! command -v genhtml &> /dev/null; then
    echo "ERROR: genhtml is not installed."
    echo "Install with: brew install lcov (macOS) or apt-get install lcov (Linux)"
    exit 1
fi

# Generate coverage report
echo "Generating coverage report..."
lcov --directory . --capture --output-file coverage.info

# Remove system headers and external code
lcov --remove coverage.info \
    '/usr/*' \
    '*/googletest/*' \
    '*/build/*' \
    '*/tests/*' \
    --output-file coverage.info

# Generate HTML report
genhtml coverage.info --output-directory coverage_html

echo ""
echo "=========================================="
echo "  Coverage Report Generated"
echo "=========================================="
echo ""
echo "Open coverage_html/index.html in your browser to view the report."
echo ""
echo "Coverage summary:"
lcov --summary coverage.info

