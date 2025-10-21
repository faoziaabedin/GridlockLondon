// code/core/Node.cpp
#include "Node.h"

Node::Node(int id, int row, int col) 
    : id(id), row(row), col(col) {
}

int Node::getId() const {
    return id;
}

int Node::getRow() const {
    return row;
}

int Node::getCol() const {
    return col;
}

bool Node::operator==(const Node& other) const {
    return id == other.id && row == other.row && col == other.col;
}