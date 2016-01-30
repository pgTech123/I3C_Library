#include "tests.h"

Tests::Tests()
{
}

#ifdef I3C_RENDER_MODULE_ENABLED
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
#endif

int Tests::readWriteV1()
{
    const char* filenameR = "camTest.i3c";
    const char* filenameW = "camTestWrittenV1.i3c";

    I3C_Read i3cFileR;
    I3C_Write i3cFileW;
    I3C_Frame frame;
    int error;

    error = i3cFileR.open(filenameR);
    if(error == I3C_SUCCESS){
        i3cFileR.read(&frame);
    }
    i3cFileR.close();

    error = i3cFileW.open(filenameW);
    if(error == I3C_SUCCESS){
        i3cFileW.writeImage(&frame, I3C_IMAGE_V1);
    }
    i3cFileW.close();

    return error;
}

int Tests::readV1WriteV2()
{
    const char* filenameR = "camTest.i3c";
    const char* filenameW = "camTestWrittenV2.i3c";

    I3C_Read i3cFileR;
    I3C_Write i3cFileW;
    I3C_Frame frame;
    int error;

    error = i3cFileR.open(filenameR);
    if(error == I3C_SUCCESS){
        i3cFileR.read(&frame);
    }
    i3cFileR.close();

    error = i3cFileW.open(filenameW);
    if(error == I3C_SUCCESS){
        i3cFileW.writeImage(&frame, I3C_IMAGE_V2);
    }
    i3cFileW.close();


    return I3C_SUCCESS;
}

int Tests::readV2WriteV1()
{
    const char* filenameR = "camTestWrittenV2.i3c";
    const char* filenameW = "camTestWrittenV2-V1.i3c";

    I3C_Read i3cFileR;
    I3C_Write i3cFileW;
    I3C_Frame frame;
    int error;

    error = i3cFileR.open(filenameR);
    if(error == I3C_SUCCESS){
        i3cFileR.read(&frame);
    }
    i3cFileR.close();

    return I3C_SUCCESS;
}
