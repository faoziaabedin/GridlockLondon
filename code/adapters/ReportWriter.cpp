// code/adapters/ReportWriter.cpp
#include "ReportWriter.h"
#include "../core/Metrics.h"

bool ReportWriter::writeCSV(const std::string& path, const Metrics& metrics) {
    // Stub implementation for Deliverable 2
    // In full implementation, this would write metrics to CSV file
    (void)path;    // Suppress unused parameter warning
    (void)metrics; // Suppress unused parameter warning
    
    return true;
}

bool ReportWriter::writeJSON(const std::string& path, const Metrics& metrics) {
    // Stub implementation for Deliverable 2
    // In full implementation, this would write metrics to JSON file
    (void)path;    // Suppress unused parameter warning
    (void)metrics; // Suppress unused parameter warning
    
    return true;
}
