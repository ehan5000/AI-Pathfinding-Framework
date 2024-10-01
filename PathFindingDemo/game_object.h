#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "geometry.h"

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture);

            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(glm::mat4 view_matrix, double current_time);

            // Getters
            inline glm::vec3 GetPosition(void) const { return position_; }
            inline float GetScale(void) const { return scale_; }
            inline float GetRotation(void) const { return angle_; }

            // Get bearing direction (direction in which the game object
            // is facing)
            glm::vec3 GetBearing(void) const;

            // Get vector pointing to the right side of the game object
            glm::vec3 GetRight(void) const;

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(float scale) { scale_ = scale; }
            void SetRotation(float angle);

            // Color modifier which can scale each element of a
            // fragment's color
            glm::vec3 GetColorModifier(void);
            void SetColorModifier(glm::vec3 color_mod);

        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            float scale_;
            float angle_;

            // Color modifier
            glm::vec3 color_mod_;

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;

    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
