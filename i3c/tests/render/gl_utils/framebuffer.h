#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <Windows.h>
#include <QOpenGLVertexArrayObject>
#include <GL/GL.h>

class FrameBuffer
{
public:
    FrameBuffer();
    ~FrameBuffer();

    void generateFBO(unsigned int width, unsigned int height);

    GLuint getRenderedTexture();
    GLuint getDepthTexture();

    void bind();
    void unbind();

private:
    void clear();

private:
    GLuint m_FBO;

    GLuint m_depthTexture;
    GLuint m_renderingTexture;

};

#endif // FRAMEBUFFER_H
