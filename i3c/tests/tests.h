#ifndef TESTS_H
#define TESTS_H


#include <QApplication>
#include <QSurfaceFormat>
#include "tests/render/basic_gl_i3c_rendertest.h"

class Tests
{
public:
    Tests();

    int renderingEngine(int argc, char *argv[]);
};

#endif // TESTS_H
