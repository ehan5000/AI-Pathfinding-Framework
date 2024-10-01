// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);
    color.rgb = vec3(0.8, 0.4, 0.01) * color_interp.r;

    // Assign color to fragment
    gl_FragColor = vec4(color.r, color.g, color.b, color.a);

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }
}
