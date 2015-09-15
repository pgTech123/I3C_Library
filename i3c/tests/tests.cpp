#include "tests.h"

Tests::Tests()
{
}

int Tests::renderingEngine(int argc, char *argv[])
{
    //Instanciate test
    Basic_GL_I3C_RenderTest *window = new Basic_GL_I3C_RenderTest();

    //Try to init OpenGL & SDL2
    if(window->initWindow("Basic GL I3C Render Test") < 0){
        delete window;
        return -1;
    }

    //Load the wanted image
    window->readI3CFile("./io_samples/imageV1.i3c");

    //Call program main loop
    window->loop();

    //Clear memory and return
    delete window;
    return 0;
}
