#ifndef TESTEDITING_H
#define TESTEDITING_H

#include "../../io/i3c_read.h"
#include "../../editing/i3c_cube.h"

class TestEditing
{
public:
    TestEditing();

    bool testWritingReading();
};

#endif // TESTEDITING_H
