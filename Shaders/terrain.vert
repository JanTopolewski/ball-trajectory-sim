#version 330 core

layout (location = 0) in vec3 aPos;

layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 camMatrix;
uniform float size;
uniform mat4 model;

void main() {
    gl_Position = camMatrix * model * vec4(aPos.x * size, aPos.y * size, aPos.z * size, 1.0f);
    color = aColor;
}