#ifndef TESTS_H
#define TESTS_H


#include <QApplication>
#include <QSurfaceFormat>
#include "tests/render/basic_gl_i3c_rendertest.h"
#include "../io/i3c_read.h"
#include "../io/i3c_write.h"

class Tests
{
public:
    Tests();

#ifdef    I3C_RENDER_MODULE_ENABLED
    int renderingEngine(int argc, char *argv[]);
#endif
    int readWriteV1();
    int readV1WriteV2();
    int readV2WriteV1();
};

#endif // TESTS_H
