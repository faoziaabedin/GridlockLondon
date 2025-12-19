// code/core/Agent.cpp
#include "Agent.h"
#include "City.h"
#include <stdexcept>

Agent::Agent(int id, NodeId origin, NodeId destination)
    : id(id), 
      origin(origin), 
      destination(destination),
      currentNode(origin),
      currentEdge(std::nullopt),
      departureTime(0),
      arrivalTime(-1),
      arrived(origin == destination),  // Already arrived if origin == destination
      stepsTaken(0) {
    if (arrived) {
        arrivalTime = 0;  // No travel time needed
    }
}

bool Agent::needsRoute() const {
    return path.empty() && !arrived;
}

bool Agent::hasArrived() const {
    return arrived;
}

void Agent::setPath(std::deque<EdgeId> p) {
    path = p;
}

void Agent::step(City& city) {
    // If already arrived, do nothing
    if (arrived) {
        return;
    }
    
    // Increment step counter (agent is attempting to move)
    stepsTaken++;
    
    // If currently on an edge, finish traversing it
    if (currentEdge.has_value()) {
        EdgeId edgeId = currentEdge.value();
        Edge& edge = city.getEdge(edgeId);
        
        // Decrement occupancy of the edge we're leaving
        city.decrementOccupancy(edgeId);
        
        // Move to the destination node of the edge
        currentNode = edge.getTo();
        
        // No longer on an edge
        currentEdge = std::nullopt;
        
        // Check if we reached destination
        if (currentNode == destination) {
            arrived = true;
            arrivalTime = stepsTaken;  // Use steps taken as arrival time
        }
        
        return;
    }
    
    // If no path and not on edge, agent is stuck
    if (path.empty()) {
        return;
    }
    
    // Try to move onto the next edge in the path
    EdgeId nextEdge = path.front();
    Edge& edge = city.getEdge(nextEdge);
    
    // Check if edge is blocked
    if (edge.isBlocked()) {
        // Agent needs to reroute (handled by SimulationController)
        path.clear();
        return;
    }
    
    // Check capacity
    int currentOcc = city.occupancy(nextEdge);
    int capacity = city.edgeCapacity(nextEdge);
    
    if (currentOcc < capacity) {
        // Move onto the edge
        path.pop_front();
        currentEdge = nextEdge;
        city.incrementOccupancy(nextEdge);
    }
    // else: wait at current node (capacity full)
    // Note: stepsTaken still increments even if waiting (time passes)
}

int Agent::getId() const {
    return id;
}

NodeId Agent::getOrigin() const {
    return origin;
}

NodeId Agent::getDestination() const {
    return destination;
}

NodeId Agent::getCurrentNode() const {
    return currentNode;
}

int Agent::getTravelTime() const {
    if (arrived && arrivalTime >= 0) {
        return arrivalTime;  // Return steps taken to arrive
    }
    // Not yet arrived - return current steps taken
    return stepsTaken;
}

std::optional<EdgeId> Agent::getCurrentEdge() const {
    return currentEdge;
}

const std::deque<EdgeId>& Agent::getPath() const {
    return path;
}