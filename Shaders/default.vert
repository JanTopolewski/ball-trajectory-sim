#version 330 core
// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
//layout (location = 1) in vec3 aColor;

//layout (location = 2) in vec2 aTex;

// outputs the color for the fragment shader
//out vec3 color;

out vec3 FragPos;
out vec3 Normal;
//out vec2 texCoord;

// Controls the scale of the vertices
//uniform float scale;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 proj;

uniform mat4 camMatrix;
uniform mat4 model;
uniform float size;

void main()
{
    vec3 scaledPos = size * aPos;
    vec4 worldPos = model * vec4(scaledPos, 1.0);
    gl_Position = camMatrix * worldPos;

    FragPos = worldPos.xyz;

    Normal = normalize(mat3(transpose(inverse(model))) * aPos);
   // Outputs the positions/coordinates of all vertices
   //gl_Position = camMatrix * model * vec4(size * aPos.x, size * aPos.y, size * aPos.z, 1.0);
   // Assigns the colors from the Vertex Data to "color"
//   color = aColor;
//   texCoord = aTex;
}