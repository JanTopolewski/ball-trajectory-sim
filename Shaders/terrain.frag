#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

uniform vec4 color;

void main()
{
    FragColor = color;
}