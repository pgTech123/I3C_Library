//Author:           Pascal Gendron
//Creation date:    August 31th 2015
//Description:      Definition of types for I3C module

#ifndef I3C_H
#define I3C_H

#include <stdio.h>
#include <mutex>

#define I3C_SUCCESS                     0
#define I3C_ERR_FILE_NOT_OPEN           1
#define I3C_ERR_COMPRESS_NOT_FOUND      2
#define I3C_INVALID_IMAGE_SIZE          3
#define I3C_FILE_CORRUPTED              4
#define I3C_STRUCT_MAP_CORRUPTED        10
#define I3C_STRUCT_PIXEL_CORRUPTED      11

#define I3C_CANNOT_FIND_CONTEXT         101

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


class Reflexion_Pixels{
public:
    char theta;
    char phi;
    char magnetude;
    char diffusion;
};


class I3C_Frame: public std::mutex{

public:
    int resolution;         // Should be base 2
    int numberOfLevels;     // resolution = 2^numberOfLevels

    int *mapAtLevel;        // mapAtLevel[numberOfLevels] => each entry = number of map at level


    //Arrays
    Pixel* pixel;           //Can be NULL if |yuv_pixel| != NULL
    //YUV_Pixels* yuv_pixel;  //NOT AN ARRAY
    //Reflexion_Pixels* reflexion_pixel;
    unsigned char* cubeMap;
    unsigned int* childCubeId;
    //Array size
    unsigned int pixelArraySize;
    unsigned int cubeMapArraySize;

    //VIDEO ONLY
    bool newFrame;

    //Change each frame to specify what has changed(to not be
    //obligated to rewrite the whole frame)
    long long *modifPixelBitmask;
    long long *modifMapBitmask;
    long long *modifChildIdBitmask;
    //TODO: worst diffPixels, worst diff map worst diff child Id (OCL mem allocation)

    I3C_Frame();
    ~I3C_Frame();
    void clear();

private:
    void init();
};



#endif // I3C_H
