#include "ws.h"

static void checkShader(GLuint handle)
{
    GLint bResult;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &bResult);
    if (bResult == GL_FALSE)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(handle, 1023, NULL, infoLog);
        Quit((char *)(std::string("shader compile failed: ") + infoLog).c_str());
    }
}

GLuint ws_create_program(const GLchar *vs, const GLchar *ps, const std::vector<const char *> &attribs)
{
    const GLchar *vertex_shader_with_version[2] = { "#version 120\n", vs };
    const GLchar *fragment_shader_with_version[2] = { "#version 120\n", ps };

    auto vertHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertHandle, 2, vertex_shader_with_version, NULL);
    glCompileShader(vertHandle);
    checkShader(vertHandle);

    auto fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragHandle, 2, fragment_shader_with_version, NULL);
    glCompileShader(fragHandle);
    checkShader(fragHandle);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertHandle);
    glAttachShader(program, fragHandle);
    int i = 0;
    for (auto attrib : attribs)
    {
        glBindAttribLocation(program, i, attrib);
        ++i;
    }
    glLinkProgram(program);

    return program;
}
