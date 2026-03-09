#include "../include/VAO.h"

VAO::VAO()
{
    glGenVertexArrays(1, &ID);
}

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
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
    VBO.Bind();
    // Configure the Vertex Attribute so that OpenGL knows how to read the VBO
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    // Enable the Vertex Attribute so that OpenGL knows to use it
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}


void VAO::Bind()
{
    glBindVertexArray(ID);
}

void VAO::Unbind()
{
    glBindVertexArray(0);
}

void VAO::Delete()
{
    glDeleteVertexArrays(1, &ID);
}

