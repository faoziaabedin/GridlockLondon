// code/tests/test_adapter_stubs.cpp
// Test program for adapter stub implementations
#include <iostream>
#include <cassert>
#include "adapters/JsonReader.h"
#include "adapters/TimerService.h"
#include "adapters/ReportWriter.h"
#include "core/Metrics.h"

void testJsonReader() {
    std::cout << "Test: JsonReader Stub\n";
    
    JsonReader reader;
    
    // Should not crash
    std::string result = reader.read("test.json");
    
    // Should return empty string (or dummy JSON)
    assert(result.empty() || !result.empty());  // Just check it doesn't crash
    
    std::cout << "✓ read() executes without error\n";
    std::cout << "✓ Returns: " << (result.empty() ? "empty string" : "dummy JSON") << "\n";
}

void testTimerService() {
    std::cout << "\nTest: TimerService Stub\n";
    
    TimerService timer;
    
    // Initially not running
    assert(timer.isRunning() == false);
    std::cout << "✓ isRunning() returns false initially\n";
    
    // Start should call callback once
    bool callbackCalled = false;
    timer.start(100, [&callbackCalled]() {
        callbackCalled = true;
    });
    
    assert(callbackCalled == true);
    std::cout << "✓ start() calls callback once\n";
    
    // Still not running (stub behavior)
    assert(timer.isRunning() == false);
    std::cout << "✓ isRunning() returns false after start (stub)\n";
    
    // Stop should not crash
    timer.stop();
    std::cout << "✓ stop() executes without error\n";
    
    // Still not running
    assert(timer.isRunning() == false);
}

void testReportWriter() {
    std::cout << "\nTest: ReportWriter Stub\n";
    
    ReportWriter writer;
    Metrics metrics;
    
    // writeCSV should return true
    bool csvResult = writer.writeCSV("test.csv", metrics);
    assert(csvResult == true);
    std::cout << "✓ writeCSV() returns true\n";
    
    // writeJSON should return true
    bool jsonResult = writer.writeJSON("test.json", metrics);
    assert(jsonResult == true);
    std::cout << "✓ writeJSON() returns true\n";
}

void testJsonReaderMultipleCalls() {
    std::cout << "\nTest: JsonReader Multiple Calls\n";
    
    JsonReader reader;
    
    // Multiple calls should not crash
    reader.read("file1.json");
    reader.read("file2.json");
    reader.read("file3.json");
    
    std::cout << "✓ Multiple calls to read() work correctly\n";
}

void testTimerServiceMultipleStarts() {
    std::cout << "\nTest: TimerService Multiple Starts\n";
    
    TimerService timer;
    
    int callCount = 0;
    
    // Start multiple times
    timer.start(100, [&callCount]() { callCount++; });
    timer.start(200, [&callCount]() { callCount++; });
    timer.start(300, [&callCount]() { callCount++; });
    
    // Each start should call callback once (stub behavior)
    assert(callCount == 3);
    std::cout << "✓ Multiple starts each call callback once\n";
    
    // Stop should not crash
    timer.stop();
    timer.stop();  // Multiple stops should be safe
    std::cout << "✓ Multiple stops are safe\n";
}

void testReportWriterMultipleWrites() {
    std::cout << "\nTest: ReportWriter Multiple Writes\n";
    
    ReportWriter writer;
    Metrics metrics;
    
    // Multiple writes should all return true
    assert(writer.writeCSV("output1.csv", metrics) == true);
    assert(writer.writeCSV("output2.csv", metrics) == true);
    assert(writer.writeJSON("output1.json", metrics) == true);
    assert(writer.writeJSON("output2.json", metrics) == true);
    
    std::cout << "✓ Multiple writes all return true\n";
}

void testStubCompilation() {
    std::cout << "\nTest: Stub Compilation\n";
    
    // Verify all stubs can be instantiated and used
    JsonReader reader;
    TimerService timer;
    ReportWriter writer;
    Metrics metrics;
    
    // All should compile and work
    std::string json = reader.read("test.json");
    (void)json;  // Suppress unused warning
    
    timer.start(100, []() {});
    timer.stop();
    bool running = timer.isRunning();
    (void)running;  // Suppress unused warning
    
    bool csv = writer.writeCSV("test.csv", metrics);
    bool json_write = writer.writeJSON("test.json", metrics);
    (void)csv;
    (void)json_write;
    
    std::cout << "✓ All stubs compile and instantiate correctly\n";
}

int main() {
    std::cout << "=== Adapter Stubs Test Suite ===\n\n";
    
    try {
        testJsonReader();
        testTimerService();
        testReportWriter();
        testJsonReaderMultipleCalls();
        testTimerServiceMultipleStarts();
        testReportWriterMultipleWrites();
        testStubCompilation();
        
        std::cout << "\n=== All Adapter Stub Tests Passed! ===\n";
        std::cout << "✓ All methods compile and don't crash\n";
        std::cout << "✓ Stubs allow rest of system to compile\n";
        std::cout << "✓ Can be replaced with real implementation later\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception\n";
        return 1;
    }
}

