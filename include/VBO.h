#ifndef OPENGL_TEST_VBO_H
#define OPENGL_TEST_VBO_H

#include <glad/glad.h>

class VBO
{
public:
    GLuint ID;

    VBO();
    VBO(GLfloat *vertices, GLsizeiptr size);
    VBO(GLfloat *vertices, GLsizeiptr size, GLenum usage);

    void Bind();
    void Unbind();
    void Delete();
};

#endif //OPENGL_TEST_VBO_H