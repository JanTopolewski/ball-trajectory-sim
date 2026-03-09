#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;
uniform float size;

void main() {
    gl_Position = camMatrix * vec4(aPos.x * size, aPos.y * size, aPos.z * size, 1.0f);
}