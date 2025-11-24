# Quick Start - Testing Guide

## Issue 5: Test Coverage Goals - Quick Commands

### Build with GoogleTest
```bash
cd code/build
cmake ..
make
```

### Run All Tests
```bash
cd code
./run_all_tests.sh
```

### Run GoogleTest Suites Only
```bash
cd code/build
ctest -R "Test$"
```

### Run Individual GoogleTest Suite
```bash
cd code/build
./test_route_planner_googletest
./test_agent_googletest
./test_metrics_googletest
./test_simulation_controller_googletest
./test_factory_pattern_googletest
```

### Generate Coverage Report
```bash
cd code
./run_coverage.sh
```

Then open `build/coverage_html/index.html` in your browser.

### View Test Documentation
- Test Plan: `code/tests/TEST_COVERAGE_PLAN.md`
- GoogleTest README: `code/tests/README_GOOGLETEST.md`
- Issue 5 Summary: `code/ISSUE_5_SUMMARY.md`

## Test Statistics

- **50+ test cases** across 5 GoogleTest suites
- **Parameterized tests** for multiple scenarios
- **Performance benchmarks** included
- **Mock objects** for isolated testing
- **>85% coverage goal** (with coverage enabled)

## What's Included

✅ GoogleTest/GoogleMock setup
✅ 5 comprehensive test suites
✅ Mock objects (MockPolicy, TestCityBuilder)
✅ Parameterized tests
✅ Performance benchmarks
✅ Coverage reporting (gcov/lcov)
✅ Complete documentation
✅ CI/CD ready (GitHub Actions)

