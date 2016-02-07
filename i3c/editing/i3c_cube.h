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

    void cube2rgb(I3C_Frame *rgbFrame);
    void cube2yuv(/*TODO*/);
    void rgb2cube(I3C_Frame *rgbFrame);
    void yuv2cube(/*TODO*/);

private:

};

//#endif

#endif // I3C_CUBE_H
