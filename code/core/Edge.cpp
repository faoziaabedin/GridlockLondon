// code/core/Edge.cpp
#include "Edge.h"

Edge::Edge(int id, NodeId from, NodeId to, double length, int capacity)
    : id(id), from(from), to(to), length(length), capacity(capacity), blocked(false) {
}

int Edge::getId() const {
    return id;
}

NodeId Edge::getFrom() const {
    return from;
}

NodeId Edge::getTo() const {
    return to;
}

double Edge::getLength() const {
    return length;
}

int Edge::getCapacity() const {
    return capacity;
}

bool Edge::isBlocked() const {
    return blocked;
}

void Edge::setBlocked(bool blocked) {
    this->blocked = blocked;
}