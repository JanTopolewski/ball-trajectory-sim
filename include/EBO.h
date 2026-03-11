#ifndef OPENGL_TEST_EBO_H
#define OPENGL_TEST_EBO_H

#include <glad/glad.h>

class EBO
{
public:
    GLuint ID;
    EBO(GLuint *indices, GLsizeiptr size);

    void Bind();
    void Unbind();
    void Delete();
};

#endif //OPENGL_TEST_EBO_H