#include "basic_gl_i3c_rendertest.h"

Basic_GL_I3C_RenderTest::Basic_GL_I3C_RenderTest(QWidget* parent)
    :GL_Window(parent)
{
    m_GLI3CElement = new GL_I3C_Element("camTest.i3c");
    m_GLI3CScene = new GL_I3C_Scene();
}

Basic_GL_I3C_RenderTest::~Basic_GL_I3C_RenderTest()
{
    this->stopGL();
    delete m_GLI3CScene;
    delete m_GLI3CElement;
}

void Basic_GL_I3C_RenderTest::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    //Attach the opengl context to an I3C scene
    m_GLI3CScene->attachGLContext(wglGetCurrentDC(), wglGetCurrentContext());
    if(!m_GLI3CScene->isUpAndRunning()){
        cerr << "An error occured in the creation of an OpenCL context..." << endl;
    }

    //Prepare textures on which we will render on
    this->generateFBO(this->width(), this->height());
    m_GLI3CScene->setRenderingTextures(m_renderedTexture, m_depthTexture);

    //Add element to scene
    m_GLI3CElement->setTransform(1,1,6);
    m_GLI3CScene->addI3CElement(m_GLI3CElement);

    //Compile shaders
    m_glProgramRtT = compileShaders(vertex_shader_sources_basic_test, fragment_shader_sources_basic_test);
    m_glProgramDisplayTexture = compileShaders(vertex_shader_RtT, fragment_shader_RtT);

    m_vertexArrayObject.create();
    m_vertexArrayObject.bind();
}

void Basic_GL_I3C_RenderTest::resizeGL(int w, int h)
{
    m_GLI3CScene->stopRender();
    this->deleteFBO();
    this->generateFBO(w, h);
    m_GLI3CScene->setRenderingTextures(m_renderedTexture, m_depthTexture);
}

void Basic_GL_I3C_RenderTest::paintGL()
{
    //Background
    /*GLfloat color[] = {1.0, 0.0, 0.0, 1.0};
    glClearBufferfv(GL_COLOR, 0, color);*/

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*    //Render on texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);   //Bind framebuffer
    glUseProgram(m_glProgramRtT);
    glDrawArrays(GL_TRIANGLES, 0, 3);                   //Render on texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);               //Unbind framebuffer
*/

    // Update I3C lookat
    static float angleX = 0;
    angleX +=0.02;
    m_GLI3CScene->lookAt(0,0,0, angleX, 0, 0);
    //cout << angleX << endl;

    //Render I3C
    m_GLI3CScene->renderI3C();                           //Render I3C elements

    //Display rendered texture
    glUseProgram(m_glProgramDisplayTexture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderedTexture);

    //TODO vertex = {};
/*
    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(0, 2, GL_FLOAT, 0,(const GLvoid*)0);
    glDisableVertexAttribArray(1);
*/

    GLint texID = glGetUniformLocation(m_glProgramDisplayTexture, "renderedTexture");
    glUniform1i(texID, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);                   //Display texture full screen*/

}

void Basic_GL_I3C_RenderTest::stopGL()
{
    //Clear memory
    m_vertexArrayObject.release();
    glDeleteProgram(m_glProgramRtT);
    glDeleteProgram(m_glProgramDisplayTexture);
    m_vertexArrayObject.destroy();

    //Delete Rendering textures
    this->deleteFBO();
}

GLuint Basic_GL_I3C_RenderTest::compileShaders(const GLchar* const* vertexShaderSources,
                                               const GLchar* const* fragmentShaderSources)
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

    //Create and compile vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertexShaderSources, NULL);
    glCompileShader(vertexShader);

    //Print compilation errors
    if(DEBUG){
        printShaderCompilationErrors(vertexShader);
    }

    //Create and compile framgent shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, fragmentShaderSources, NULL);
    glCompileShader(fragmentShader);

    //Print compilation errors
    if(DEBUG){
        printShaderCompilationErrors(fragmentShader);
    }

    //Create program & linking
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    //Clear what's not needed anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Basic_GL_I3C_RenderTest::printShaderCompilationErrors(GLuint shader)
{
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE){
        cerr << "vertex compilation error..." << endl;

        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        for (int i = 0; i < maxLength; i++){
            clog << errorLog[i];
        }
        cout << endl;
    }
}

void Basic_GL_I3C_RenderTest::generateFBO(unsigned int width, unsigned int height)
{
    //Generate Frame Buffer Object
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    //Generate Rendering Texture
    glGenTextures(1, &m_renderedTexture);
    glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    //Generate Depth Texture
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    //Bind textures to pipeline
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

    //Set the list of draw buffers.
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);      // "1" is the size of DrawBuffers

    //Check if everything went ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "Error! FrameBuffer is not complete" << endl;
    }

    //Unbind FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Basic_GL_I3C_RenderTest::deleteFBO()
{
    glDeleteFramebuffers(1, &m_frameBuffer);
    glDeleteTextures(1, &m_renderedTexture);
    glDeleteTextures(1, &m_depthTexture);
}
