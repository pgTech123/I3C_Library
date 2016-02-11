//Author:           Pascal Gendron
//Creation date:    Febuary 3th 2016
//Description:      Definition of the I3C Editing Object

#ifndef I3C_CUBE_H
#define I3C_CUBE_H

//#ifdef I3C_EDITING_MODULE_ENABLED

#include "i3c.h"
#include "editing/i3c_editingcube.h"
#include "../../../utils/binary_func.h"


#define DEFAULT_CUBE_WIDTH       512

#define I3C_CUBEEDIT_INCOMPATIBLE_WIDTH     201
#define I3C_CUBEEDIT_NOT_SET                202
#define I3C_CUBEEDIT_INVALID_POS            203


class I3C_Cube
{
public:
    I3C_Cube(int width=DEFAULT_CUBE_WIDTH);
    ~I3C_Cube();

    int addPixel(int x, int y, int z, Pixel pixel);
    int removePixel(int x, int y, int z);
    int getPixelAt(int x, int y, int z, Pixel* pixel);

    void resetEditingCube(int resolution = -1);

    void cube2rgb(I3C_Frame *rgbFrame);
    void cube2yuv(/*TODO*/);
    int rgb2cube(I3C_Frame *rgbFrame);
    void yuv2cube(/*TODO*/);

    int getWidth();

private:
    I3C_EditingCube *m_editingCube;
    int m_width;

};

//#endif

#endif // I3C_CUBE_H
