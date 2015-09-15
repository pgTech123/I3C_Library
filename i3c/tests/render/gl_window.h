#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include <stdio.h>

#include <SDL2/SDL.h>   //UI (multiplatform)
#include <SDL2/SDL_opengl.h>

#ifdef __APPLE__            //APPLE OS HAS NOT BEEN TESTED YET
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32               //WINDOWS OS HAS NOT BEEN TESTED YET
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif


#define WINDOW_WIDTH                800   //px
#define WINDOW_HEIGHT               600   //px
#define GL_CONTEXT_MAJOR_VERSION    3
#define GL_CONTEXT_MINOR_VERSION    3
#define DEFAULT_ERROR_FILE          "log/error.txt"
#define DEFAULT_LOG_FILE            "log/log.txt"

class GL_Window
{
public:
    GL_Window();
    GL_Window(const char* logFileName, const char* errorFileName);
    virtual ~GL_Window();

    int initWindow(const char* windowName);
    void loop();

protected:
    virtual bool exit();        //Default: exit when "escape" clicked

    virtual void initGL();
    virtual void renderGL();
    virtual void stopGL();

private:
    void quitWindow();

protected:
    //Log files
    FILE* m_p_logFile;
    FILE* m_p_errorFile;

    //SDL Window & GL context
    SDL_Window* m_gl_Window;
    SDL_GLContext m_gl_Context;

    //Events
    SDL_Event m_sld_event;
};

#endif // GL_WINDOW_H
