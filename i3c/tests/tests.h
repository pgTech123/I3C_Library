#ifndef TESTS_H
#define TESTS_H


#include <QApplication>
#include <QSurfaceFormat>
#include "tests/render/basic_gl_i3c_rendertest.h"
#include "tests/editing/testediting.h"
#include "../utils/logs.h"

#include "../io/i3c_read.h"
#include "../io/i3c_write.h"

#define I3C_TESTS_R1W1      0x0000001
#define I3C_TESTS_R1W2      0x0000002
#define I3C_TESTS_R2W1      0x0000004
#define I3C_TESTS_RENDER    0x0000008
#define I3C_TESTS_EDITING   0x0000010


class Tests
{
public:
    Tests();

    static int runTests(int testsToRun);

#ifdef    I3C_RENDER_MODULE_ENABLED
    static int renderingEngine(int argc, char *argv[]);
#endif
//#ifdef   I3C_EDITING_MODULE_ENABLED
    static int testEditing();
//#endif

    static int readWriteV1();
    static int readV1WriteV2();
    static int readV2WriteV1();
};

#endif // TESTS_H
