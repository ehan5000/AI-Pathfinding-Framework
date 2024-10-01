// Source code of vertex shader for particle system
#version 130

// Vertex buffer
in vec2 vertex; // Vertex coordinates
in vec2 dir; // Velocity
in float t; // Phase
in vec2 uv; // Texture coordinates

// Uniform (global) buffer
uniform mat4 transformation_matrix;
uniform mat4 view_matrix;
uniform float time; // Timer

// Attributes forwarded to the fragment shader
out vec4 color_interp;
out vec2 uv_interp;

void main()
{
    vec4 pos; // Vertex position
    float cycle = 2.0; // Duration of cycle in seconds
    float speed = 4.0; // Speed adjustment constant
    float gravity = 2.8; // Gravity in this world
    float acttime; // Cyclic time

    // Add phase to the time and cycle it
    acttime = mod(time + t*cycle, cycle);

    // Move particle along given direction
    pos = vec4(vertex.x + acttime*speed*dir.x , vertex.y + acttime*speed*dir.y , 0.0, 1.0);

    // Add wobble effect
    //pos = vec4(vertex.x + cos(acttime)*speed*dir.x , vertex.y + sin(acttime)*speed*dir.y , 0.0, 1.0);

    // Add gravity
    //pos = vec4(vertex.x+dir.x*acttime*speed , vertex.y+dir.y*acttime*speed + 0.5*gravity*acttime*acttime, 0.0, 1.0);

    // No motion, for debug
    //pos = vec4(vertex.x, vertex.y, 0.0, 1.0);

    // Transform vertex position
    gl_Position = view_matrix*transformation_matrix*pos;
    
    // Set color
    //color_interp = vec4(0.5+0.5*cos(4*acttime),0.5*sin(4*acttime)+0.5,0.5, 1.0);
    color_interp = vec4(t, 0.0, 0.0, 1.0);

    // Transfer texture coordinates
    uv_interp = uv;
}
