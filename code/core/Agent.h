// code/core/Agent.h
#pragma once
#include "Types.h"
#include <optional>
#include <deque>
#include <cstddef>

class City;

class Agent {
public:
    Agent(int id, NodeId origin, NodeId destination);
    
    // Route management
    bool needsRoute() const;
    bool hasArrived() const;
    void setPath(std::deque<EdgeId> p);
    
    // Movement
    void step(City& city);
    
    // Getters
    int getId() const;
    NodeId getOrigin() const;
    NodeId getDestination() const;
    NodeId getCurrentNode() const;
    int getTravelTime() const;
    std::optional<EdgeId> getCurrentEdge() const;
    const std::deque<EdgeId>& getPath() const;
    
private:
    int id;
    NodeId origin;
    NodeId destination;
    NodeId currentNode;
    std::optional<EdgeId> currentEdge;
    std::deque<EdgeId> path;
    int departureTime;
    int arrivalTime;
    bool arrived;
    int stepsTaken;  // Track number of steps taken for travel time calculation
};