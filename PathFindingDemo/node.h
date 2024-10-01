#ifndef NODE_H_
#define NODE_H_

#include <vector>

namespace game {

// Forward declaration of the node class
class Node;

// An edge between two nodes
struct Edge {
    Node *n1; // This is always the source node
    Node *n2; // This is always the target node
    float cost; // Cost of traversing the edge
};

// A node in a graph
class Node {

    public:
        // Create a node at position (x, y)
        Node(int id, float x, float y);

        // Connects two nodes with an edge that has the given cost
        //
        // Note that this method automatically adds the symmetric edge
        // to the other node
        void AddNeighbor(Node *n, float edge_cost);

        // Connects two nodes together with a given edge
        inline void AddEdge(const Edge &e) { edge_.push_back(e); }

        // Get neighborhood information for this node
        inline int GetNumEdges(void) { return edge_.size(); }
        inline const Edge &GetEdge(int index) { return edge_[index]; }
       
        // Getters for node properties
        inline int GetId(void) const { return id_; }
        inline float GetX(void) const { return x_; }
        inline float GetY(void) const { return y_; }
        inline bool GetVisited(void) const { return visited_; }
        inline float GetCost(void) const { return cost_; }
        inline bool IsOnPath(void) const { return on_path_; }
        inline Node* GetPrev(void) { return prev_; }

        // Setters for node properties
        inline void SetX(float x) { x_ = x; }
        inline void SetY(float y) { y_ = y; }
        inline void SetPosition(float x, float y) { x_ = x; y_ = y; }
        inline void SetVisited(bool visited) { visited_ = visited; }
        inline void SetCost(float cost) { cost_ = cost; }
        inline void SetOnPath(bool path_status) { on_path_ = path_status; }
        inline void SetPrev(Node* node) { prev_ = node; }

    protected:
        // Vector containing all edges the node connects to
        // This can be used to create a graph where nodes have any
        // number of neighbors
        std::vector<Edge> edge_;

        // Unique id of the node
        const int id_;

        // Position of the node
        float x_, y_;

        // Flag for graph traversals
        bool visited_;

        // Members for computing shortest paths

        // Cost of the node
        float cost_;

        // Node is on path
        bool on_path_;

        // Link to previous node on path
        Node* prev_;
}; 

} // namespace game

#endif // NODE_H_
