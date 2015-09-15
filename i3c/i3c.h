//Author:           Pascal Gendron
//Creation date:    August 31th 2015
//Description:      Definition of types for I3C module

#ifndef I3C_H
#define I3C_H

#include <stdio.h>

#define I3C_SUCCESS                     0
#define I3C_ERR_FILE_NOT_FOUND          1
#define I3C_ERR_COMPRESS_NOT_FOUND      2
#define I3C_INVALID_IMAGE_SIZE          3
#define I3C_FILE_CORRUPTED              4

struct Pixel{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

class YUV_Pixels{

public:
    unsigned char* Y;
    unsigned char* U;
    unsigned char* V;

    YUV_Pixels();
    ~YUV_Pixels();
    void clear();

private:
    void init();
};


class I3C_Frame{

public:
    int resolution;
    //Arrays
    Pixel* pixel;           //Can be NULL if |yuv_pixel| != NULL
    YUV_Pixels* yuv_pixel;  //NOT AN ARRAY
    unsigned char* cubeMap;
    unsigned int* childCubeId;
    //Array size
    unsigned int pixelArraySize;
    unsigned int cubeMapArraySize;

    I3C_Frame();
    ~I3C_Frame();
    void clear();

private:
    void init();
};


class I3C_VideoFrame{

public:
    I3C_Frame frame;    //Larger than Image to fit displacement
    //Change each frame to specify what has changed(to not be
    //obligated to rewrite the whole frame)
    long long modifPixelBitmask;
    long long modifMapBitmask;
    long long modifChildIdBitmask;

    I3C_VideoFrame();
    ~I3C_VideoFrame();
    void clear();
};


#endif // I3C_H
