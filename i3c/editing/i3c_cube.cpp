#include "i3c_cube.h"

//#ifdef I3C_EDITING_MODULE_ENABLED

I3C_Cube::I3C_Cube(int resolution): I3C_EditingCube(resolution)
{
}

I3C_Cube::~I3C_Cube()
{
}

void I3C_Cube::cube2rgb(I3C_Frame *rgbFrame)
{
    rgbFrame->resolution = m_width;
    rgbFrame->numberOfLevels = firstHighBit(m_width);

    //TODO
}

void I3C_Cube::cube2yuv(/*TODO*/)
{
    //TODO
}

int I3C_Cube::rgb2cube(I3C_Frame *rgbFrame)
{
    if(rgbFrame->resolution != m_width){
        return I3C_CUBEEDIT_INCOMPATIBLE_WIDTH;
    }
}

void I3C_Cube::yuv2cube(/*TODO*/)
{
    //TODO
}

int I3C_Cube::getWidth()
{
    return m_width;
}

//#endif
