#ifndef I3C_CUBE_H
#define I3C_CUBE_H

//#ifdef I3C_EDITING_MODULE_ENABLED

#include "i3c.h"
#include "editing/i3c_editingcube.h"


#define DEFAULT_CUBE_WIDTH       512


class I3C_Cube: public I3C_EditingCube
{
public:
    I3C_Cube(int width=DEFAULT_CUBE_WIDTH);
    ~I3C_Cube();

    void cube2rgb();
    void cube2yuv();
    void rgb2cube();
    void yuv2cube();

private:

};

//#endif

#endif // I3C_CUBE_H
