// code/adapters/PresetLoader.h
#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../core/Preset.h"

class City;
class Agent;

/**
 * PresetLoader builds City and spawns Agents from Preset configuration.
 * Acts as an adapter between JSON/configuration and domain objects.
 */
class PresetLoader {
public:
    /**
     * Load a Preset from a JSON file.
     * For Deliverable 2, this can be a stub that returns a default preset.
     * @param path Path to JSON file
     * @return Preset object loaded from JSON
     */
    Preset loadFromJson(const std::string& path);
    
    /**
     * Build a City from a Preset configuration.
     * Creates grid topology and applies blocked edges.
     * @param preset Preset configuration
     * @return Unique pointer to created City
     */
    std::unique_ptr<City> buildCity(const Preset& preset);
    
    /**
     * Spawn agents based on Preset configuration.
     * Creates agents with random origins and destinations.
     * @param preset Preset configuration
     * @param city Reference to the City (needed for validation)
     * @return Vector of unique pointers to Agents
     */
    std::vector<std::unique_ptr<Agent>> spawnAgents(const Preset& preset, City& city);

    /**
     * Create grid topology for a city.
     * Creates nodes and bidirectional edges connecting neighbors.
     * @param rows Number of rows in the grid
     * @param cols Number of columns in the grid
     * @return Unique pointer to City with grid topology
     */
    std::unique_ptr<City> createGridTopology(int rows, int cols);
    
    /**
     * Apply blocked edges to a city.
     * Marks specified edges as blocked.
     * @param city Reference to the City
     * @param blockedEdges Vector of (from, to) node pairs to block
     */
    void applyBlockedEdges(City& city, const std::vector<std::pair<NodeId, NodeId>>& blockedEdges);
};
