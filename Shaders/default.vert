#version 330 core
// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
//layout (location = 1) in vec3 aColor;

//layout (location = 2) in vec2 aTex;

// outputs the color for the fragment shader
//out vec3 color;

//out vec2 texCoord;

// Controls the scale of the vertices
//uniform float scale;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 proj;

uniform mat4 camMatrix;

void main()
{
   // Outputs the positions/coordinates of all vertices
   gl_Position = camMatrix * vec4(aPos, 1.0); // vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
   // Assigns the colors from the Vertex Data to "color"
//   color = aColor;
//   texCoord = aTex;
}