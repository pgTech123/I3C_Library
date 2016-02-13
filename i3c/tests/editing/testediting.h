#ifndef TESTEDITING_H
#define TESTEDITING_H

#include "../../io/i3c_read.h"
#include "../../io/i3c_write.h"
#include "../../editing/i3c_cube.h"
#include "../../utils/logs.h"

#define I3C_EDITING_FAIL_RESOLUTION     -1
#define I3C_EDITING_FAIL_NUMOFLEVEL     -2
#define I3C_EDITING_FAIL_MAPARRAYSIZE   -3
#define I3C_EDITING_FAIL_PIXARRAYSIZE   -4
#define I3C_EDITING_FAIL_MAPATLEVEL     -5
#define I3C_EDITING_FAIL_MAP            -6
#define I3C_EDITING_FAIL_PIX            -7
#define I3C_EDITING_FAIL_CHILDID        -8


#define TEST_FILE_IN        "camTest.i3c"
#define TEST_FILE_OUT       "camTestOut.i3c"
#define TEST_DRAWING_OUT    "draw.i3c"


class TestEditing
{
public:
    TestEditing();

    int testWritingReading();
    int testDrawing();
};

#endif // TESTEDITING_H
