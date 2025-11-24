# Running Creational Patterns Tests

## Quick Start

```bash
# 1. Build the project using Qt Creator
#    (File → Open File or Project → code/CMakeLists.txt → Build)

# 2. Run the tests
cd build
./test_creational_patterns
```

## Test Executable

The test executable is: `build/test_creational_patterns`

## What Gets Tested

✅ **Factory Pattern** (6 tests)
- Regular, Random, and RealWorld grid factories
- Polymorphism and edge cases
- Grid connectivity

✅ **Builder Pattern** (8 tests)
- Fluent interface and method chaining
- Validation and error handling
- Edge cases and default values

✅ **Abstract Factory Pattern** (8 tests)
- Policy factories and registry
- Singleton pattern
- Error handling and extensibility

✅ **Integration Tests** (3 tests)
- All patterns working together
- Builder + Factory integration
- Registry + Builder integration

**Total: 25+ comprehensive test cases**

## Expected Output

```
********* Start testing of TestCreationalPatterns *********
Config: Using QtTest library 6.9.3, Qt 6.9.3
PASS   : TestCreationalPatterns::testRegularGridFactory()
PASS   : TestCreationalPatterns::testRandomGridFactory()
PASS   : TestCreationalPatterns::testRealWorldGridFactory()
PASS   : TestCreationalPatterns::testFactoryPolymorphism()
PASS   : TestCreationalPatterns::testFactoryEdgeCases()
PASS   : TestCreationalPatterns::testFactoryGridConnectivity()
PASS   : TestCreationalPatterns::testPresetBuilderBasic()
PASS   : TestCreationalPatterns::testPresetBuilderFluentInterface()
PASS   : TestCreationalPatterns::testPresetBuilderValidation()
PASS   : TestCreationalPatterns::testPresetBuilderReset()
PASS   : TestCreationalPatterns::testPresetBuilderBlockedEdges()
PASS   : TestCreationalPatterns::testPresetBuilderEdgeCases()
PASS   : TestCreationalPatterns::testPresetBuilderIsValid()
PASS   : TestCreationalPatterns::testPresetBuilderDefaultValues()
PASS   : TestCreationalPatterns::testShortestPathFactory()
PASS   : TestCreationalPatterns::testCongestionAwareFactory()
PASS   : TestCreationalPatterns::testPolicyRegistrySingleton()
PASS   : TestCreationalPatterns::testPolicyRegistryCreateByName()
PASS   : TestCreationalPatterns::testPolicyRegistryCreateByType()
PASS   : TestCreationalPatterns::testPolicyRegistryGetAvailable()
PASS   : TestCreationalPatterns::testPolicyRegistryGetInfo()
PASS   : TestCreationalPatterns::testPolicyRegistryExtensibility()
PASS   : TestCreationalPatterns::testPolicyRegistryErrorHandling()
PASS   : TestCreationalPatterns::testPolicyFactoryPolymorphism()
PASS   : TestCreationalPatterns::testAllPatternsIntegration()
PASS   : TestCreationalPatterns::testBuilderWithFactory()
PASS   : TestCreationalPatterns::testRegistryWithBuilder()
Totals: 25 passed, 0 failed, 0 skipped
********* Finished testing of TestCreationalPatterns *********
```

## Troubleshooting

### Test executable not found
```bash
# Make sure you built the project first
cd build
ls -la test_creational_patterns
# If not found, build using Qt Creator
```

### Compilation errors
- Make sure all pattern files are in `code/patterns/`
- Check that `CMakeLists.txt` includes all pattern files
- Verify Qt6 is installed: `qmake6 --version`

### Test failures
- Check the error message for specific test failure
- Verify pattern implementations are correct
- Check that all dependencies are linked

## Test Details

See `TEST_CREATIONAL_PATTERNS_SUMMARY.md` for detailed test descriptions.

## Integration with Other Tests

These tests are part of the complete test suite. Run all tests:

```bash
cd code
./run_all_tests.sh
```

The creational patterns tests will be included in the full test run.

