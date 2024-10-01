#include <glm/gtc/matrix_transform.hpp> 
#include <queue>
#include <algorithm>
#include <iostream>   
#include <stack>

#include "graph.h"

namespace game {

Graph::Graph(void){

    // Initialize all members to default values
    node_obj_ = NULL;
    edge_obj_ = NULL;
    start_node_ = NULL;
    end_node_ = NULL;
    hover_node_ = NULL;
}


Node *Graph::AddNode(int id, float x, float y){

    // Create and add new node to the graph
    Node *node = new Node(id, x, y);
    node_.push_back(node);
    return node;
}


void Graph::BuildEmptyGraph(GameObject *node_sprite, GameObject *edge_sprite){

    // Set sprite game objects
    node_obj_ = node_sprite;
    edge_obj_ = edge_sprite;
}


void Graph::BuildSimpleGraph(GameObject *node_sprite, GameObject *edge_sprite){

    // Set sprite game objects
    node_obj_ = node_sprite;
    edge_obj_ = edge_sprite;

    // Create a graph with only five nodes
    Node *n0 = AddNode(0, -2.0, 0.0);
    Node *n1 = AddNode(1, -1.0, 0.0);
    Node *n2 = AddNode(2,  0.0, 0.0);
    Node *n3 = AddNode(3,  1.0, 0.0);
    Node *n4 = AddNode(4,  2.0, 0.0);

    // Connect nodes
    n0->AddNeighbor(n1, 1.0); // Symmetric edge is added automatically
    n1->AddNeighbor(n2, 1.0);
    n2->AddNeighbor(n3, 1.0);
    n3->AddNeighbor(n4, 1.0);

    // Set default start and end nodes
    SetStartNode(n0);
    SetEndNode(n4);

    // Find shortest path between nodes
    FindPath();
}


void Graph::BuildGrid(int cols, int rows, float disp_x, float disp_y, float start_x, float start_y, float viewport_height, GameObject *node_sprite, GameObject *edge_sprite){

    // Set sprite game objects
    node_obj_ = node_sprite;
    edge_obj_ = edge_sprite;

    // Add nodes in a grid layout
    int id = 0;
    float x = start_x;
    float y = viewport_height - start_y;
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            AddNode(id, x, y);
            id++;
            x += disp_x;
        }
        x = start_x;
        y -= disp_y;
    }

    // Connect neighbors
    int index = 0;
    int rand_weight = 0;
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            // Only need to add neighbors to the right and bottom, as
            // the other neighbors are added automatically by
            // AddNeighbor

            // Right neighbor
            // Do not add neighbor if at the last column
            if (j < (cols-1)){
                // Chose a random weight
                // You can also try setting all weights to 1
                rand_weight = 10 + (rand() % 6);
                node_[index]->AddNeighbor(node_[index+1], rand_weight);
            }

            // Bottom neighbor
            // Do not add neighbor if at the last row
            if (i < (rows-1)){
                rand_weight = 10 + (rand() % 6);
                node_[index]->AddNeighbor(node_[(i+1)*cols+j], rand_weight);
            }

            // Increment index of current node
            index++;
        }
    }

    // Set default start and end nodes
    SetStartNode(node_[0]);
    SetEndNode(node_[rows*cols-1]);

    // Find shortest path between nodes
    FindPath();
}


void Graph::PrintData() {

    // Loop through array and print out data for each node
    for (int i = 0; i < node_.size(); i++) {
        std::cout << "Node " << i << ": id: " << node_[i]->GetId() << ", x: " << node_[i]->GetX() << ", y: " << node_[i]->GetY() << ", number of neighbors: " << node_[i]->GetNumEdges() << std::endl;
    }
}


void Graph::Update(GLFWwindow *window, float zoom){

    // Get mouse pixel position in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get information about the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Find node at the given pixel position
    Node *n = SelectNode(xpos, ypos, width, height, zoom);

    // Set the hover node to this node
    // It is fine if the node is NULL
    hover_node_ = n;

    // Check mouse clicks
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        
        // Set the start to selected node, if node exists and is not the end-node
        if (n != NULL && n != end_node_) {
            SetStartNode(n);
        }

        // Find a path between currently selected nodes
        FindPath();
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {

        // Set the end to selected node, if node exists and is not the start-node
        if (n != NULL && n != start_node_) {
            SetEndNode(n);
        }

        // Find a path between currently selected nodes
        FindPath();
    }
}


Node *Graph::SelectNode(double x, double y, int window_width, int window_height, float camera_zoom){

    // If the mouse is outside the window, return NULL
    if (x < 0 || x > window_width || y < 0 || y > window_height) {
        return NULL;
    }

    // Get position in world coordinates corresponding to the mouse
    // pixel position on the window
    float w = window_width;
    float h = window_height;
    float cursor_x_pos = 0.0;
    float cursor_y_pos = 0.0;
    if (w > h){
        float aspect_ratio = w/h;
        cursor_x_pos = ((2.0f*x - w)*aspect_ratio)/(w*camera_zoom);
        cursor_y_pos = (-2.0f*y + h)/(h*camera_zoom);
    } else {
        float aspect_ratio = h/w;
        cursor_x_pos = (2.0f*x - w)/(w*camera_zoom);
        cursor_y_pos = ((-2.0f*y + h)*aspect_ratio)/(h*camera_zoom);
    }

    // Find node at the derived position
    // This is done in a brute-force manner by checking each node, and
    // can be improved with spatial query structures
    float node_scale = node_obj_->GetScale();
    for (int i = 0; i < node_.size(); i++){
        // Check if mouse is inside a ball of radius scale*scale
        glm::vec2 center(node_[i]->GetX(), node_[i]->GetY());
        glm::vec2 cursor(cursor_x_pos, cursor_y_pos);
        if (glm::length(cursor - center) < node_scale*node_scale){
            return node_[i];
        }
    }

    // Return NULL by default (no node found)
    return NULL;
}


void Graph::Render(glm::mat4 view_matrix, double current_time){

    // First, render all the nodes in the graph so that they appear on
    // top of the edges
    //
    // Go through each node and render it using the provided game object
    for (int i = 0; i < node_.size(); i++) {
        
        // Get the current node to draw
        Node *current_node = node_[i];

        // Set the position of the sprite with the position of the node
        glm::vec3 pos(current_node->GetX(), current_node->GetY(), 0.0f);
        node_obj_->SetPosition(pos);
        
        // Set the color of the node via the color modifier uniform
        // The default color is green
        node_obj_->SetColorModifier(glm::vec3(0.0f, 0.6f, 0.0f)); // Dark green

        // Change the color modifier uniform depending on whether the
        // node is the start or end node of the path, a node in the
        // middle of the path, or the mouse is hovering over the node
        if (current_node == start_node_) {
            node_obj_->SetColorModifier(glm::vec3(1.0f, 0.0f, 0.0f)); // Red
        } else if (current_node == end_node_) {
            node_obj_->SetColorModifier(glm::vec3(0.0f, 0.0f, 1.0f)); // Blue
        } else if (current_node == hover_node_) {
            node_obj_->SetColorModifier(glm::vec3(1.0f, 0.6f, 1.0f)); // Pink
        } else if (current_node->IsOnPath()) {
            node_obj_->SetColorModifier(glm::vec3(0.0f, 1.0f, 0.0f)); // Light green
        }
        
        // Render the game object for the current node
        node_obj_->Render(view_matrix, current_time);
    }

    // Now, render all the edges in the graph
    for (int i = 0; i < node_.size(); i++) {
        
        // Get the current node to draw
        Node *current_node = node_[i];

        // Render the edges of this node
        for (int i = 0; i < current_node->GetNumEdges(); i++) {
            // Get pointer to neighbor edge
            const Edge edge = current_node->GetEdge(i);
            Node *neigh = edge.n2;

            // Set the position of the edge sprite between the current
            // node and its neighbor
            glm::vec3 pos((current_node->GetX() + neigh->GetX())/2.0, 
                          (current_node->GetY() + neigh->GetY())/2.0, 0.0f);
            edge_obj_->SetPosition(pos);

            // Check if the edge needs to be rotated
            if ((neigh->GetY() > current_node->GetY()) || 
                (neigh->GetY() < current_node->GetY())){
                edge_obj_->SetRotation(glm::pi<float>()/2.0);
            } else {
                edge_obj_->SetRotation(0.0);
            }

            // Set the color of the edge via the color modifier uniform
            // Default color
            edge_obj_->SetColorModifier(glm::vec3(0.0f, 0.6f, 0.0f)); // Dark green

            // Change the color modifier uniform depending on whether
            // the edge is on the path or not
            if (current_node->IsOnPath() && neigh->IsOnPath()) {
                edge_obj_->SetColorModifier(glm::vec3(0.0f, 1.0f, 0.0f)); // Lighter green
            }

            // Render the game object for the current edge
            edge_obj_->Render(view_matrix, current_time);
        }
    }
}


// Structure used for ranking nodes in the priority queue
// Declared here as it is only used in the FindPath() method below
struct QNode{
    Node *node; // Pointer to node
    float cost; // Cost of node
};


// Class used for comparing two QNode structures
class CompareNode
{
    public:
        int operator() (const QNode& n1, const QNode& n2)
        {
            return n1.cost > n2.cost;
        }
};



void Graph::FindPath(void){

    // Initialize the priority queue used in path finding
    // It is created using the QNode struct with a min compare class called CompareNode
    std::priority_queue<QNode, std::vector<QNode>, CompareNode> pq;
    // Clear current path
    path_node_.clear();
    // Set the costs of all nodes to infinity
    // Reset all nodes to be off-path
    for (int i = 0; i < node_.size(); i++) {
        node_[i]->SetCost(INT_MAX);
        node_[i]->SetOnPath(false);
    }

    // The start node is added to the priority queue with cost 0
    QNode temp = {start_node_, 0};
    pq.push(temp);

    // Set the cost of the starting node
    start_node_->SetCost(0.0);
    
    // Now that the pq is initialized, we can start the algorithm
    while (!pq.empty()) {
        // Get the current lowest-cost node in the pq
        QNode lowest = pq.top();

        // Remove the lowest node from the queue after retrieving it
        pq.pop(); 
        
        // If the current node is the end node, we are done
        if (lowest.node == end_node_) {
            break;
        }

        // Otherwise, check the neighbors of the lowest node
        for (int i = 0; i < lowest.node->GetNumEdges(); i++){

            // Compute cost to get to neighbouring node
            // cost = the cost to get the current node + cost to traverse the edge
            const Edge edge = lowest.node->GetEdge(i);
            Node *n = edge.n2;
            float node_cost = lowest.cost + edge.cost;

            // If the new cost is smaller than the current node cost,
            // update the node cost, and add an updated QNode to the pq
            if (node_cost < n->GetCost()){
                // Update node cost
                n->SetCost(node_cost);    

                // Update the shortest path to the node
                n->SetPrev(lowest.node);
                
                // Add zombie node to update value of node in the queue
                QNode updated_node = {n, node_cost};
                pq.push(updated_node);
            }
        }

    }

    // Queue is done, go in reverse from END to START to determine path
    Node* current_node = end_node_->GetPrev();
    
    // While the current node isn't null, or the end, mark the current
    // node as on the path
    path_node_.push_back(end_node_);
    while (current_node != NULL && current_node != start_node_) {
        path_node_.push_back(current_node);
        current_node->SetOnPath(true);
        current_node = current_node->GetPrev();
    }
    path_node_.push_back(current_node);
    // Reverse path to get the order from start to end
    std::reverse(path_node_.begin(), path_node_.end());

    // Also set the start and end nodes to be on the path for display
    // purposes
    start_node_->SetOnPath(true);
    end_node_->SetOnPath(true);

    // Uncomment to see the ids in order on the path 
    ///for (Node *ele : path_node_) {
    //    std::cout << "id:" << ele->GetId() << std::endl;
    //}/
}


void Graph::BuildMaze(Graph& output){

    // Copy all the nodes to the output graph
    for (int i = 0; i < node_.size(); i++) {
        output.AddNode(node_[i]->GetId(), node_[i]->GetX(), node_[i]->GetY());
    }

    // Perform a depth search to add edges to the output graph, creating
    // a maze structure in this process

    // Initialize all nodes as not visited
    for (int i = 0; i < node_.size(); i++) {
        node_[i]->SetVisited(false);
    }

    // Initialize a stack
    std::stack<Node *> st;

    // Add the first node to the stack
    st.push(node_[0]);
    node_[0]->SetVisited(true);

    // Depth-first search
    while (st.size() > 0){
        // Retrieve top element from the stack
        Node *n = st.top();

        // Create a randomized list of neighbors	
        std::vector<int> index;
        for (int i = 0; i < n->GetNumEdges(); i++) {
            index.push_back(i);
        }
        std::random_shuffle(index.begin(), index.end());

        // Find an unvisited neighbor
        bool found_unvisited = false;
        for (int i = 0; i < index.size(); i++) {
            const Edge edge = n->GetEdge(index[i]);
            Node *neigh = edge.n2;

            if (!neigh->GetVisited()){
                // Add connection to unvisited neighbor in the output graph
                // Assumes the id of a node corresponds to its index
                Node *n1 = output.GetNode(n->GetId());
                Node *n2 = output.GetNode(neigh->GetId());
                n1->AddNeighbor(n2, edge.cost);

                // Mark node as visited
                neigh->SetVisited(true);

                // Add neighbor to the stack
                st.push(neigh);

                // Set flag that a node was found
                found_unvisited = true;
                break;
            }
        }

        // If no unvisited neighbor was found, remove the node from the
        // stack
        if (!found_unvisited){
            st.pop();
        }
    }

    // Set default start and end nodes for output graph
    output.SetStartNode(output.GetNode(0));
    output.SetEndNode(output.GetNode(output.GetNumNodes()-1));

    // Find shortest path between nodes in the output graph
    output.FindPath();
}

} // namespace game
