#include "../include/VBO.h"

VBO::VBO() : ID(0){}

VBO::VBO(GLfloat *vertices, GLsizeiptr size)
{
    glGenBuffers(1, &ID);
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    // Introduce the vertices into the VBO
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VBO::VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage)
{
    glGenBuffers(1, &ID);
    // Bind the VBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    // Introduce the vertices into the VBO
    glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
    glDeleteBuffers(1, &ID);
}

