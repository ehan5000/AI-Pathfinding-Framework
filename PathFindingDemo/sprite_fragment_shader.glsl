// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;

// Color modifier: we multiply each component of the color by each
// component of this modifier
uniform vec3 color_mod;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);

    // Assign color to fragment
    color *= vec4(color_mod, 1.0);
    gl_FragColor = color;

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }
}
