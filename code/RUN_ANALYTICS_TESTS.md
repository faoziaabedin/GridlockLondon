# Running Analytics Tests - Quick Guide

## Prerequisites

**IMPORTANT**: The test executable must be built first using Qt Creator.

## Step 1: Build the Project

### Option A: Using Qt Creator (Recommended)

1. Open Qt Creator
2. File → Open File or Project
3. Select: `code/CMakeLists.txt`
4. Configure Project (select Qt 6.9.3 kit)
5. Build → Build Project (Cmd+B / Ctrl+B)

### Option B: Check if Already Built

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ls -la test_analytics
```

If the file exists, you can run it. If not, build with Qt Creator.

## Step 2: Run the Tests

### From the build directory:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
./test_analytics
```

### From the code directory:

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code
./build/test_analytics
```

### Using CTest (if configured):

```bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build
ctest -R test_analytics
```

## Expected Output

```
********* Start testing of TestAnalytics *********
Config: Using QtTest library 6.9.3, Qt 6.9.3
PASS   : TestAnalytics::initTestCase()
PASS   : TestAnalytics::testHotspotDetection()
PASS   : TestAnalytics::testFlowData()
PASS   : TestAnalytics::testUtilizationHeatmap()
PASS   : TestAnalytics::testTopBottlenecks()
PASS   : TestAnalytics::testCalculateUtilization()
PASS   : TestAnalytics::testPolicyComparison()
PASS   : TestAnalytics::testStatisticalSignificance()
PASS   : TestAnalytics::testConfidenceInterval()
PASS   : TestAnalytics::testHypothesisTest()
PASS   : TestAnalytics::testEfficiencyScore()
PASS   : TestAnalytics::testCalculatePValue()
PASS   : TestAnalytics::testTStatistic()
PASS   : TestAnalytics::testTrendProjection()
PASS   : TestAnalytics::testCompletionEstimate()
PASS   : TestAnalytics::testBottleneckPrediction()
PASS   : TestAnalytics::testAgentRecommendation()
PASS   : TestAnalytics::testLinearRegression()
PASS   : TestAnalytics::testExponentialSmoothing()
PASS   : TestAnalytics::testExportCSV()
PASS   : TestAnalytics::testExportJSON()
PASS   : TestAnalytics::testSaveJSON()
PASS   : TestAnalytics::cleanupTestCase()
Totals: 25 passed, 0 failed, 0 skipped
********* Finished testing of TestAnalytics *********
```

## Troubleshooting

### "test_analytics: No such file or directory"

**Solution**: Build the project first using Qt Creator.

### "Permission denied"

**Solution**: 
```bash
chmod +x test_analytics
```

### Tests fail to compile

**Solution**: 
1. Check that all analytics source files are in `CMakeLists.txt`
2. Verify Qt6::Charts is linked
3. Rebuild in Qt Creator

### CMake recursion error

**Solution**: Use Qt Creator to build (workaround for Qt6 CMake bug)

## Quick Check Script

```bash
#!/bin/bash
cd /Users/parimehla/Desktop/fao+pri/oop-assignment-2-faozia-and-paridhi/code/build

if [ -f test_analytics ]; then
    echo "✅ Test executable found"
    echo "Running tests..."
    ./test_analytics
else
    echo "❌ Test executable not found"
    echo "Please build the project using Qt Creator first:"
    echo "  1. Open Qt Creator"
    echo "  2. File → Open → code/CMakeLists.txt"
    echo "  3. Build → Build Project"
fi
```

