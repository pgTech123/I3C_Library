#include "gl_window.h"

GL_Window::GL_Window(QWidget* parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    setFormat(format);

    openLogFiles(DEFAULT_LOG_FILE, DEFAULT_ERROR_FILE);
    initRefreshRate(REFRESH_RATE_MS);
}

GL_Window::~GL_Window()
{
    this->quitWindow();
}

void GL_Window::openLogFiles(const char* logFileName, const char* errorFileName)
{
    /*m_p_logFile = fopen(logFileName, "w+");
    m_p_errorFile = fopen(errorFileName, "w+");*/
}

void GL_Window::initRefreshRate(int interval_ms)
{
    m_refreshTimer.start(30);
    connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
}


void GL_Window::quitWindow()
{
    //Print exit ok!
    /*fprintf(m_p_logFile, "SDL quitted properly.\n");

    //Close files
    fclose (m_p_logFile);
    fclose (m_p_errorFile);*/
}
