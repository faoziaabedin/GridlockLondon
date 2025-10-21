// code/core/Node.h
#pragma once

class Node {
public:
    Node(int id, int row, int col);
    
    // Getters
    int getId() const;
    int getRow() const;
    int getCol() const;
    
    // Comparison operator
    bool operator==(const Node& other) const;
    
private:
    int id;
    int row;
    int col;
};