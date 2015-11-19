#include "tests.h"

Tests::Tests()
{
}

int Tests::renderingEngine(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    //Instanciate test
    Basic_GL_I3C_RenderTest window;
    window.setFormat(format);
    window.show();

    return a.exec();
}
