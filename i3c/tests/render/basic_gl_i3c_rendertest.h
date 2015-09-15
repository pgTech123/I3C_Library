#ifndef BASIC_GL_I3C_RENDERTEST_H
#define BASIC_GL_I3C_RENDERTEST_H

#include <iostream>
#include <vector>
using namespace std;

#include "gl_window.h"

#include "../../io/i3c_read.h"
#include "../../i3c.h"

#define DEBUG           1           //1 = DEBUG MODE, 0 = RELEASE MODE


class Basic_GL_I3C_RenderTest: public GL_Window
{
public:
    Basic_GL_I3C_RenderTest();

    void readI3CFile(const char* filename);

protected:
    virtual void initGL();
    virtual void renderGL();
    virtual void stopGL();

private:
    GLuint compileShaders();
    void printShaderCompilationErrors(GLuint shader);

private:
    GLuint m_glProgram;
    GLuint m_vertexArrayObject;

    I3C_Read m_i3c_reader;
    I3C_Frame m_i3c_frame;
};

#endif // BASIC_GL_I3C_RENDERTEST_H
