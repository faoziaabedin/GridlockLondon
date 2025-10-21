// code/core/Edge.h
#pragma once
#include "Types.h"

class Edge {
public:
    Edge(int id, NodeId from, NodeId to, double length, int capacity);
    
    // Getters
    int getId() const;
    NodeId getFrom() const;
    NodeId getTo() const;
    double getLength() const;
    int getCapacity() const;
    
    // Blocked status
    bool isBlocked() const;
    void setBlocked(bool blocked);
    
private:
    int id;
    NodeId from;
    NodeId to;
    double length;
    int capacity;
    bool blocked;
};