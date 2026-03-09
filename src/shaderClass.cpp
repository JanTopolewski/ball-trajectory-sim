#include "../Include/shaderClass.h"

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    throw (errno);
}

Shader::Shader(const char *vertexFile, const char *fragmentFile)
{
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Create vertex shader and get its reference
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // attach vertex shader source to vertex shader object
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    // compile vertex shader into machine code
    glCompileShader(vertexShader);
    // Checks if Shader compiled succesfully
    compileErrors(vertexShader, "VERTEX");

    // Create fragment shader and get its reference
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Attach Fragment Shader source to the Fragment Shader Object
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    // Compile the Vertex Shader into machine code
    glCompileShader(fragmentShader);
    // Checks if Shader compiled succesfully
    compileErrors(fragmentShader, "FRAGMENT");

    // (linking) create the shader program and attach compiled shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    // Checks if Shaders linked succesfully
    compileErrors(ID, "PROGRAM");

    // after creating and linking the shader's program, the two separate shaders are useless and can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}

// Checks if the different Shaders have compiled properly
void Shader::compileErrors(unsigned int shader, const char *type)
{
    // Stores status of compilation
    GLint hasCompiled;
    // Character array to store the message in
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, sizeof(infoLog), NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
        }
    }
}
