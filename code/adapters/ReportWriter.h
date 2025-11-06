// code/adapters/ReportWriter.h
#pragma once

#include <string>

class Metrics;

/**
 * ReportWriter - Writes metrics to files.
 * Stub implementation for Deliverable 2.
 * Full implementation will write CSV/JSON files in Deliverable 3.
 */
class ReportWriter {
public:
    /**
     * Write metrics to CSV file.
     * @param path Path to output CSV file
     * @param metrics Metrics to write
     * @return true if successful, false otherwise
     */
    bool writeCSV(const std::string& path, const Metrics& metrics);
    
    /**
     * Write metrics to JSON file.
     * @param path Path to output JSON file
     * @param metrics Metrics to write
     * @return true if successful, false otherwise
     */
    bool writeJSON(const std::string& path, const Metrics& metrics);
};

