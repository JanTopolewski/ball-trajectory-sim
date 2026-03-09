#ifndef OPENGL_TEST_VAO_H
#define OPENGL_TEST_VAO_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
    GLuint ID;
    VAO();
    /**
     * @brief
     * Links a VBO Attribute such as a position or color to the VAO
     *
     * @param VBO The Vertex Buffer Object to link to
     * @param layout to which layout in shader to bind the elements
     * @param numComponents how many elements are we binding now
     * @param type what type are the elements
     * @param stride how many bytes are between the beginnings of the elements
     * @param offset where do elements start
     */
    void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
    void Bind();
    void Unbind();
    void Delete();
};

#endif //OPENGL_TEST_VAO_H