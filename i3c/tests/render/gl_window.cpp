#include "gl_window.h"

GL_Window::GL_Window()
{
    m_gl_Window = NULL;
    m_p_logFile = fopen(DEFAULT_LOG_FILE, "w+");
    m_p_errorFile = fopen(DEFAULT_ERROR_FILE, "w+");
}

GL_Window::GL_Window(const char* logFileName, const char* errorFileName)
{
    m_gl_Window = NULL;
    m_p_logFile = fopen(logFileName, "w+");
    m_p_errorFile = fopen(errorFileName, "w+");
}
GL_Window::~GL_Window()
{
    this->quitWindow();
}

int GL_Window::initWindow(const char *windowName)
{
    //Set OpenGL Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_CONTEXT_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_CONTEXT_MINOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    //Init SDL & OpenGL
    if((SDL_Init(SDL_INIT_VIDEO) < 0)) {
        fprintf(m_p_errorFile, "Could not initialize SDL: %s.\n", SDL_GetError());
        return -1;
    }

    //Create the Window
    m_gl_Window = SDL_CreateWindow( windowName,
                                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    WINDOW_WIDTH, WINDOW_HEIGHT,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    fprintf(m_p_logFile, "SDL Window created.\n");

    //Get the GL context
    m_gl_Context = SDL_GL_CreateContext(m_gl_Window);
    if(m_gl_Context == NULL){
        fprintf(m_p_errorFile ,"OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;
    }
    else{
        fprintf(m_p_logFile, "OpenGL context was created\n");
    }

    //Init
    initGL();

    return 0;
}

void GL_Window::loop()
{
    while(!exit())
    {
        //Render
        this->renderGL();
    }
}

void GL_Window::quitWindow()
{
    SDL_GL_DeleteContext(m_gl_Context);
    SDL_DestroyWindow(m_gl_Window);
    SDL_Quit();

    //Print exit ok!
    fprintf(m_p_logFile, "SDL quitted properly.\n");

    //Close files
    fclose (m_p_logFile);
    fclose (m_p_errorFile);
}

void GL_Window::initGL()
{
    GLfloat color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glClearBufferfv(GL_COLOR, 0, color);
    SDL_GL_SwapWindow(m_gl_Window);
}

void GL_Window::renderGL()
{
}

void GL_Window::stopGL()
{
}

bool GL_Window::exit()
{
    SDL_WaitEvent(&m_sld_event);
    if (m_sld_event.type == SDL_KEYDOWN && m_sld_event.key.keysym.sym == SDLK_ESCAPE){
        this->stopGL();
        return true;
    }
    return false;
}
