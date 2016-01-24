#include "basic_gl_i3c_rendertest.h"

Basic_GL_I3C_RenderTest::Basic_GL_I3C_RenderTest(QWidget* parent)
    :GL_Window(parent)
{
    //"Elsa_512.i3c"   "camTest.i3c"    "1_plane.i3c"    "1_cube.i3c"    "2_plane.i3c"
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

    //Attach GL context to I3C
    this->GL_I3C_init();

    //Prepare textures on which we will render on
    this->generateFBO(this->width(), this->height());
    m_GLI3CScene->setRenderingTextures(m_renderedTexture, m_depthTexture);

    //Setup I3C scene on top of GL 'scene'
    this->setI3CScene();

    //Compile shaders
    m_glProgramRenderGLtoTexture = compileShaders(VS_renderTriangleToTexture, FS_renderTriangleToTexture);
    m_glProgramDisplayTexture = compileShaders(VS_renderTextureOnScreen, FS_renderTextureOnScreen);

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
    //Update I3C image
    static float angleX = 0;
    angleX +=0.02;
    m_GLI3CElement->setTransform(0,1,3.5, 2*angleX, angleX*2,3*angleX);
    m_GLI3CScene->lookAt(0,0,0, 0.2, 0.3, 0);

    this->renderToTexture();
    this->displayTextureOnScreen();
}

void Basic_GL_I3C_RenderTest::stopGL()
{
    //Make sure nothing is running anymore on GPU
    m_GLI3CScene->stopRender();

    //Clear memory
    m_vertexArrayObject.release();
    glDeleteProgram(m_glProgramRenderGLtoTexture);
    glDeleteProgram(m_glProgramDisplayTexture);
    m_vertexArrayObject.destroy();

    //Delete Rendering textures
    this->deleteFBO();
}

void Basic_GL_I3C_RenderTest::GL_I3C_init()
{
    //Attach the opengl context to an I3C scene
    m_GLI3CScene->attachGLContext(wglGetCurrentDC(), wglGetCurrentContext());
    if(!m_GLI3CScene->isUpAndRunning()){
        cerr << "An error occured in the creation of an OpenCL context..." << endl;
    }
}

void Basic_GL_I3C_RenderTest::setI3CScene()
{
    //Add & remove element to scene (partly tests :P)
    m_GLI3CElement->setTransform(1,1,4);
    m_GLI3CScene->addI3CElement(m_GLI3CElement);
    m_GLI3CScene->retreiveAll();
    m_GLI3CScene->addI3CElement(m_GLI3CElement);
    m_GLI3CScene->retreiveI3CElement(m_GLI3CElement);
    m_GLI3CScene->addI3CElement(m_GLI3CElement);
}

void Basic_GL_I3C_RenderTest::renderToTexture()
{
    //Render GL to texture
    glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
    glActiveTexture(GL_TEXTURE0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);   //Bind framebuffer so that we render on texture
    glViewport(0, 0, this->width(), this->height());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat color[] = {0.0, 0.0, 0.0, 1.0};
    glClearBufferfv(GL_COLOR, 0, color);

    glUseProgram(m_glProgramRenderGLtoTexture);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //Render I3C
    m_GLI3CScene->renderI3C();                           //Render I3C elements
}

void Basic_GL_I3C_RenderTest::displayTextureOnScreen()
{
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());   //Bind qt framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, this->width(), this->height());

    glUseProgram(m_glProgramDisplayTexture);
    GLint texID = glGetUniformLocation(m_glProgramDisplayTexture, "renderedTexture");
    glUniform1i(texID, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
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
    glGenRenderbuffers(1, &m_depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthTexture);//*/
    /*glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);//*/

    //Bind textures to pipeline
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

    //Set the list of draw buffers.
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    //Check if everything went ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        logs << "GL_error: Basic_GL_I3C_RenderingTest --- generateFBO could not complete creation of textures" << endl;
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
