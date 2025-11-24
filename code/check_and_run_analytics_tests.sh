#!/bin/bash

echo "=========================================="
echo "  Analytics Tests - Quick Check"
echo "=========================================="
echo ""

cd "$(dirname "$0")/build" 2>/dev/null || {
    echo "❌ Build directory not found!"
    echo ""
    echo "Please build the project first using Qt Creator:"
    echo "  1. Open Qt Creator"
    echo "  2. File → Open File or Project"
    echo "  3. Select: code/CMakeLists.txt"
    echo "  4. Configure Project (select Qt 6.9.3 kit)"
    echo "  5. Build → Build Project (Cmd+B)"
    echo ""
    exit 1
}

if [ -f test_analytics ]; then
    echo "✅ Test executable found!"
    echo ""
    echo "Running analytics tests..."
    echo "=========================================="
    echo ""
    ./test_analytics
    exit $?
else
    echo "❌ test_analytics executable not found!"
    echo ""
    echo "The test hasn't been built yet."
    echo ""
    echo "Please build the project using Qt Creator:"
    echo "  1. Open Qt Creator"
    echo "  2. File → Open File or Project"
    echo "  3. Select: code/CMakeLists.txt"
    echo "  4. Configure Project (select Qt 6.9.3 kit)"
    echo "  5. Build → Build Project (Cmd+B)"
    echo ""
    echo "After building, run this script again."
    exit 1
fi
