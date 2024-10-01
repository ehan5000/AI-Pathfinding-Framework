#ifndef GRAPH_H_
#define GRAPH_H_

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "node.h"
#include "shader.h"
#include "game_object.h"

namespace game {

// A graph with connected nodes
class Graph {

    public:
        // Lightweight constructor
        Graph(void);

        // Add a node to the graph
        Node *AddNode(int id, float x, float y);

        // Build an empty graph
        void BuildEmptyGraph(GameObject *node_sprite, GameObject *edge_sprite);

        // Build a simple graph for demo purposes
        void BuildSimpleGraph(GameObject *node_sprite, GameObject *edge_sprite);

        // Build a graph with a grid layout
        // Parameters: columns and rows of the grid, displacement of
        // each node along x and y, starting x and y position, viewport
        // height and sprite information
        void BuildGrid(int cols, int rows, float disp_x, float disp_y, float start_x, float start_y, float viewport_height, GameObject *node_sprite, GameObject *edge_sprite);

        // Print out associated data for each node in the graph
        void PrintData(void);

        // Get mouse input, update start and end node, and compute
        // shortest path between the two nodes
        void Update(GLFWwindow *window, float zoom);

        // Return the node at the (x, y) coordinate of the window
        Node *SelectNode(double x, double y, int window_width, int window_height, float camera_zoom);

        // Return the node at the given index
        inline Node *GetNode(int index) { return node_[index]; }
        inline int GetNumNodes(void) { return node_.size(); }

        // Render all the nodes in the graph
        void Render(glm::mat4 view_matrix, double current_time);

        // Create and mark a path from start to end
        void FindPath(void);
 
        // Getters
        inline Node *GetStartNode(void) { return start_node_; }
        inline Node *GetEndNode(void) { return end_node_; }

        // Setters
        inline void SetStartNode(Node *node) { start_node_ = node; }
        inline void SetEndNode(Node *node) { end_node_ = node; }

        // Maze generation
        // Take the current graph as input, generate a graph with a
        // maze, and return the new graph as the output
        void BuildMaze(Graph& output);

    private:
        // Sprite used to draw each node
        GameObject *node_obj_;

        // Sprite used to draw each edge
        GameObject *edge_obj_;

        // Vector containing all the nodes in the graph
        std::vector<Node*> node_;

        // Node that the mouse is hovering over
        Node *hover_node_;

        // Members for computing shortest paths in the graph

        // Start and end nodes of a path
        Node *start_node_;
        Node *end_node_;

        // Nodes in current shortest path
        std::vector<Node*> path_node_;
};

} // namespace game

#endif // GRAPH_H_
