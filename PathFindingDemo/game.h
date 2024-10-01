#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "game_object.h"
#include "graph.h"

namespace game {

    // A class for holding the main game objects
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void); 

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            // Sprite geometry
            Geometry *sprite_;

            // Particle geometry
            Geometry *particles_;

            // Shader for rendering sprites in the scene
            Shader sprite_shader_;

            // Shader for rendering particles
            Shader particle_shader_;

            // References to textures
            // This needs to be a pointer
            GLuint *tex_;

            // List of game objects
            std::vector<GameObject*> game_objects_;

            // Keep track of time
            double current_time_;

            // Camera zoom level
            float camera_zoom_;

            // Cool down control for pressing zoom in/out keys
            float time_since_last_zoom_;
            float time_for_next_zoom_;

            // Graph for traversal of game world
            Graph g_;

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname);

            // Load all textures
            void SetAllTextures();

            // Handle user input
            void HandleControls(double delta_time);

            // Update all the game objects
            void Update(double delta_time);
 
            // Render the game world
            void Render(void);

    }; // class Game

} // namespace game

#endif // GAME_H_
