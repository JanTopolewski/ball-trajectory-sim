#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the color from the Vertex Shader
//in vec3 color;

//in vec2 texCoord;

//uniform sampler2D tex0;
uniform vec4 color;

void main()
{
   FragColor = color;
//   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//   FragColor = texture(tex0, texCoord);
}