#include <stdexcept>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>

#include <path_config.h>

#include "sprite.h"
#include "particles.h"
#include "shader.h"
#include "player_game_object.h"
#include "particle_system.h"
#include "game.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "PathFinding Demo";
const unsigned int window_width_g = 1024;
const unsigned int window_height_g = 768;
const glm::vec3 viewport_background_color_g(0.4, 0.4, 0.4);

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;


Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void)
{

    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set whether window can be resized
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    sprite_->CreateGeometry();

    // Initialize particle geometry
    particles_ = new Particles();
    particles_->CreateGeometry();

    // Initialize particle shader
    particle_shader_.Init((resources_directory_g+std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/particle_fragment_shader.glsl")).c_str());

    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    // Initialize time
    current_time_ = 0.0;

    // Zoom cool down control
    time_since_last_zoom_ = 0.0;
    time_for_next_zoom_ = 0.25;
}


Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    delete particles_;
    for (int i = 0; i < game_objects_.size(); i++){
        delete game_objects_[i];
    }

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{

    // Setup the game world

    // Load textures
    SetAllTextures();

    // Set up zoom level
    camera_zoom_ = 0.25f;

    // Setup sprite used as graph node
    GameObject *node_sprite = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[0]);
    node_sprite->SetScale(0.5);

    // Setup sprite used as graph edge
    GameObject *edge_sprite = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[1]);
    edge_sprite->SetScale(0.5);

    // Setup graph

    // Choose one of the options with the pre-processor flag below
#define GRAPH_OPTION 2
    // Option 1: simple graph
    // Option 2: grid graph
    // Option 3: grid graph + maze

#if GRAPH_OPTION == 1

    // Simple graph
    g_.BuildSimpleGraph(node_sprite, edge_sprite);

#elif GRAPH_OPTION == 2

    // Graph in grid format
    //
    // If the window's aspect ratio is 1024/768 = 1.33 and the global
    // zoom factor is 0.25, then the window ranges from (-5.33, -4.0) to (5.334, 4.0)
    // So, we lay out the grid graph over this range
    // We add a small shift to start_x and start_y so that the graph is
    // not glued to the window's edge
    g_.BuildGrid(18, 14, 0.5, 0.5, -4.25, 0.75, 4, node_sprite, edge_sprite);

#elif GRAPH_OPTION == 3

    // Grid graph + maze
    Graph temp;
    temp.BuildGrid(18, 14, 0.5, 0.5, -4.25, 0.75, 4, node_sprite, edge_sprite);
    g_.BuildEmptyGraph(node_sprite, edge_sprite);
    temp.BuildMaze(g_);
#endif
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    if (!image){
        std::cout << "Cannot load texture " << fname << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    // Declare all the textures here
    const char *texture[] = {"/textures/node.png", "/textures/edge.png"};
    // Get number of declared textures
    int num_textures = sizeof(texture) / sizeof(char *);
    // Allocate a buffer for all texture references
    tex_ = new GLuint[num_textures];
    glGenTextures(num_textures, tex_);
    // Load each texture
    for (int i = 0; i < num_textures; i++){
        SetTexture(tex_[i], (resources_directory_g+std::string(texture[i])).c_str());
    }
    // Set first texture in the array as default
    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}


void Game::MainLoop(void)
{
    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update window events like input handling
        glfwPollEvents();

        // Handle user input
        HandleControls(delta_time);

        // Update all the game objects
        Update(delta_time);

        // Render all the game objects
        Render();

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);
    }
}


void Game::HandleControls(double delta_time){

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    } else if (glfwGetKey(window_, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        if (time_since_last_zoom_ >= time_for_next_zoom_){
            camera_zoom_ *= 1.5; // Zoom in
            time_since_last_zoom_ = 0.0;
        }
    } else if (glfwGetKey(window_, GLFW_KEY_MINUS) == GLFW_PRESS) {
        if (time_since_last_zoom_ >= time_for_next_zoom_){
            camera_zoom_ /= 1.5; // Zoom out
            time_since_last_zoom_ = 0.0;
        }
    } else if (glfwGetKey(window_, GLFW_KEY_R) == GLFW_PRESS) {
        if (time_since_last_zoom_ >= time_for_next_zoom_){
            camera_zoom_ = 0.25; // Reset zoom
            time_since_last_zoom_ = 0.0;
        }
    }
}


void Game::Update(double delta_time)
{

    // Update time
    current_time_ += delta_time;

    // Update all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        // Get the current game object
        GameObject* current_game_object = game_objects_[i];

        // Update the current game object
        current_game_object->Update(delta_time);

        // Check for collision with other game objects
        // Note the loop bounds: we avoid testing the last object since
        // it's the background covering the whole game world
        for (int j = i + 1; j < (game_objects_.size()-1); j++) {
            GameObject* other_game_object = game_objects_[j];

            // Compute distance between object i and object j
            float distance = glm::length(current_game_object->GetPosition() - other_game_object->GetPosition());
            // If distance is below a threshold, we have a collision
            if (distance < 0.8f) {
                // This is where you would perform collision response between objects
            }
        }
    }

    // Update the graph
    g_.Update(window_, camera_zoom_);

    // Cool down for zoom
    time_since_last_zoom_ += delta_time;
}


void Game::Render(void){

    // Clear background
    glClearColor(viewport_background_color_g.r,
                 viewport_background_color_g.g,
                 viewport_background_color_g.b, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use aspect ratio to properly scale the window
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    glm::mat4 window_scale_matrix;
    if (width > height){
        float aspect_ratio = ((float) width)/((float) height);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/aspect_ratio, 1.0f, 1.0f));
    } else {
        float aspect_ratio = ((float) height)/((float) width);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f/aspect_ratio, 1.0f));
    }

    // Set view to zoom out, centered by default at 0,0
    glm::mat4 camera_zoom_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom_, camera_zoom_, camera_zoom_));
    glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix;

    // Render all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        game_objects_[i]->Render(view_matrix, current_time_);
    }

    // Render the graph
    g_.Render(view_matrix, current_time_);
}
      
} // namespace game
