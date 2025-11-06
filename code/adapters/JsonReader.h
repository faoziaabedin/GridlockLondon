// code/adapters/JsonReader.h
#pragma once

#include <string>

/**
 * JsonReader - Thin JSON wrapper.
 * Stub implementation for Deliverable 2.
 * Full implementation will parse JSON files in Deliverable 3.
 */
class JsonReader {
public:
    /**
     * Read JSON content from a file.
     * @param path Path to JSON file
     * @return JSON content as string (empty string for stub)
     */
    std::string read(const std::string& path);
};

