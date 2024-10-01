#include <climits>

#include "node.h"

namespace game {

Node::Node(int id, float x, float y) : id_(id) {

    // Initialize private members
    // edge_ is automatically initialized
    x_ = x;
    y_ = y;
    visited_ = false;
    cost_ = INT_MAX;
    on_path_ = false;
    prev_ = nullptr;
}


void Node::AddNeighbor(Node *n, float edge_cost) {

    // Creates an edge corresponding to the specified parameters
    Edge e = { this, n, edge_cost };

    // Add the edge to the vector in this node
    this->AddEdge(e);

    // Add the edge to the vector in the other node
    e.n1 = n;
    e.n2 = this;
    n->AddEdge(e);
}

} // namespace game
