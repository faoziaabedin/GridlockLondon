// code/patterns/PresetBuilder.h
#pragma once

#include "../core/Preset.h"
#include <string>
#include <vector>

/**
 * Builder Pattern: Complex Preset Builder
 * 
 * Provides a fluent interface for constructing Preset objects.
 * This pattern is useful because Preset has many optional parameters,
 * and we want to validate the configuration before building.
 * 
 * Benefits:
 * - Fluent interface (method chaining)
 * - Validation before building
 * - Clearer than constructor with 10+ parameters
 * - Can build presets incrementally
 */
class PresetBuilder {
public:
    PresetBuilder();
    ~PresetBuilder() = default;
    
    /**
     * Set the preset name.
     * @param name Name identifier for the preset
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& setName(const std::string& name);
    
    /**
     * Set the grid dimensions.
     * @param rows Number of rows
     * @param cols Number of columns
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& setGridSize(int rows, int cols);
    
    /**
     * Set the number of agents.
     * @param count Number of agents to spawn
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& setAgentCount(int count);
    
    /**
     * Set the routing policy.
     * @param type Policy type (SHORTEST_PATH or CONGESTION_AWARE)
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& setPolicy(PolicyType type);
    
    /**
     * Add a blocked edge.
     * @param from Source node ID
     * @param to Destination node ID
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& addBlockedEdge(NodeId from, NodeId to);
    
    /**
     * Add multiple blocked edges.
     * @param edges Vector of (from, to) pairs
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& addBlockedEdges(const std::vector<std::pair<NodeId, NodeId>>& edges);
    
    /**
     * Set the tick interval (simulation speed).
     * @param ms Milliseconds per tick
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& setTickInterval(int ms);
    
    /**
     * Reset the builder to default state.
     * @return Reference to this builder for method chaining
     */
    PresetBuilder& reset();
    
    /**
     * Build and validate the Preset.
     * @return Validated Preset object
     * @throws std::runtime_error if validation fails
     */
    Preset build();
    
    /**
     * Check if the current configuration is valid (without building).
     * @return true if configuration is valid
     */
    bool isValid() const;
    
private:
    std::string name;
    int rows;
    int cols;
    int agentCount;
    PolicyType policy;
    std::vector<std::pair<NodeId, NodeId>> blockedEdges;
    int tickMs;
    
    /**
     * Create a Preset from current builder state.
     */
    Preset createPreset() const;
};

