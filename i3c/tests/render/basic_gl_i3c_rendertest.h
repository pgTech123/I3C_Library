#ifndef BASIC_GL_I3C_RENDERTEST_H
#define BASIC_GL_I3C_RENDERTEST_H

#include <iostream>
#include <vector>
using namespace std;

#include "gl_window.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QImage>

#include "../../io/i3c_read.h"
#include "../../i3c.h"
#include "../../render/gl_i3c_scene.h"
#include "../../render/gl_i3c_element.h"
#include "./../../utils/logs.h"

#define DEBUG           1           //1 = DEBUG MODE, 0 = RELEASE MODE


static const GLchar* VS_renderTriangleToTexture[] =
{
    "#version 330 core                                                      \n"
    "                                                                       \n"
    "void main(void)                                                        \n"
    "{                                                                      \n"
    "   const vec4 vertices[3] = vec4[3](vec4( 0.50, -0.50, 0.5, 1.0),      \n"
    "                                    vec4(-0.50, -0.50, 0.5, 1.0),      \n"
    "                                    vec4( 0.50,  0.50, 0.5, 1.0));     \n"
    "                                                                       \n"
    "   gl_Position = vertices[gl_VertexID];                                \n"
    "}                                                                      \n"
};


static const GLchar* FS_renderTriangleToTexture[] =
{
    "#version 330 core                                                      \n"
    "                                                                       \n"
    "layout(location = 0) out vec4 color;                                   \n"
    "                                                                       \n"
    "void main(void)                                                        \n"
    "{                                                                      \n"
    "   color = vec4(1.0, 1.0, 0.0, 1.0);                                   \n"
    "}                                                                      \n"
};

static const GLchar* VS_renderTextureOnScreen[]=
{
    "#version 330 core                                                      \n"
    "                                                                       \n"
    "                                                                       \n"
    "layout(location = 0) in vec2 vertexUV;                                 \n"
    "                                                                       \n"
    "out vec2 texcoord;                                                     \n"
    "                                                                       \n"
    "void main(void)                                                        \n"
    "{                                                                      \n"
    "   const vec4 vertices[6] = vec4[6](vec4(-1.0, -1.0, 0.0, 1.0),        \n"
    "                                    vec4( 1.0, -1.0, 0.0, 1.0),        \n"
    "                                    vec4(-1.0,  1.0, 0.0, 1.0),        \n"
    "                                    vec4(-1.0,  1.0, 0.0, 1.0),        \n"
    "                                    vec4( 1.0, -1.0, 0.0, 1.0),        \n"
    "                                    vec4( 1.0,  1.0, 0.0, 1.0));       \n"
    "                                                                       \n"
    "   gl_Position = vertices[gl_VertexID];                                \n"
    "   texcoord = vertexUV;                                           \n"
    "}                                                                      \n"
};

static const GLchar* FS_renderTextureOnScreen[]=
{
    "#version 330 core                                                                          \n"
    "                                                                                           \n"
    "in vec2 texcoord;                                                                          \n"
    "                                                                                           \n"
    "out vec4 color;                                                                            \n"
    "                                                                                           \n"
    "uniform sampler2D renderedTexture;                                                         \n"
    "                                                                                           \n"
    "void main(){                                                                               \n"
    "   color = texelFetch(renderedTexture, ivec2(gl_FragCoord.xy),0);                        \n"
    //"   color = texture(renderedTexture, texcoord);                        \n"
    "}                                                                                          \n"
};

class Basic_GL_I3C_RenderTest: public GL_Window
{
public:
    Basic_GL_I3C_RenderTest(QWidget* parent = 0);
    ~Basic_GL_I3C_RenderTest();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    void GL_I3C_init();
    void setI3CScene();

    void renderToTexture();
    void displayTextureOnScreen();

    GLuint compileShaders(const GLchar* const* vertexShader, const GLchar* const* fragmentShader);
    void printShaderCompilationErrors(GLuint shader);

    void stopGL();

    void generateFBO(unsigned int width, unsigned int height);
    void deleteFBO();

private:
    //I3C
    GL_I3C_Element *m_GLI3CElement;
    GL_I3C_Scene *m_GLI3CScene;

    GLuint m_glProgramRenderGLtoTexture;
    GLuint m_glProgramDisplayTexture;

    QOpenGLVertexArrayObject m_vertexArrayObject;

    I3C_Read m_I3CReader;
    I3C_Frame m_I3CFrame;

    //Rendering texture
    GLuint m_frameBuffer;
    GLuint m_renderedTexture;
    GLuint m_depthTexture;
};


#endif // BASIC_GL_I3C_RENDERTEST_H
