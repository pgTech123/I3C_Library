#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include <stdio.h>

#include <QSurfaceFormat>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include <QTimer>

#define DEFAULT_ERROR_FILE          "log/error.txt"
#define DEFAULT_LOG_FILE            "log/log.txt"
#define REFRESH_RATE_MS             30 //ms

class GL_Window: public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{

    Q_OBJECT

public:
    GL_Window(QWidget* parent = 0 );
    virtual ~GL_Window();

private:
    void openLogFiles(const char* logFileName, const char* errorFileName);
    void initRefreshRate(int interval_ms);
    void quitWindow();

protected:
    //Log files
    FILE* m_p_logFile;
    FILE* m_p_errorFile;

private:
    QTimer m_refreshTimer;
};

#endif // GL_WINDOW_H
