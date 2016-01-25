#include "i3c.h"


I3C_Frame::I3C_Frame():std::mutex()
{
    init();
}

void I3C_Frame::init()
{
    resolution = 0;
    mapAtLevel = NULL;
    pixel = NULL;
    //yuv_pixel = NULL;
    cubeMap = NULL;
    childCubeId = NULL;
    pixelArraySize = 0;
    cubeMapArraySize = 0;
}

I3C_Frame::~I3C_Frame()
{
    this->clear();
}

void I3C_Frame::clear()
{
    if(mapAtLevel!= NULL){
        delete[] mapAtLevel;
    }
    if(pixel != NULL){
        delete[] pixel;
    }
    /*if(yuv_pixel != NULL){
        delete yuv_pixel;
    }*/
    if(cubeMap != NULL){
        delete[] cubeMap;
    }
    if(childCubeId != NULL){
        delete[] childCubeId;
    }
    init();
}
