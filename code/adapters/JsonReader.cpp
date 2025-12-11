// code/adapters/JsonReader.cpp
#include "JsonReader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string JsonReader::read(const std::string& path) {
    std::ifstream file(path);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}
