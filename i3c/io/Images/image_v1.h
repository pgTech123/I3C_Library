//Author:           Pascal Gendron
//Creation date:    September 5th 2015
//Description:      Image V1 Reading/Writing algorithm

#ifndef IMAGE_V1_H
#define IMAGE_V1_H

#include <fstream>
#include <iostream>
#include <string>

#include "../../i3c.h"
#include "../../utils/binary_func.h"

using namespace std;


class ImageV1
{
public:
    ImageV1();
    ~ImageV1();

    int read(fstream* file, I3C_Frame* frame);
    int write(fstream* file, I3C_Frame* frame);

private:
    int readSideSize(fstream* file);
    int readPixels(fstream* file, I3C_Frame* frame);
    int readMap(fstream* file, unsigned char* ucMap, int* iNumOfPix);
    int readParents(fstream* file, I3C_Frame* frame);

    void writeSideSize(fstream* file, I3C_Frame* frame);
    void writeMapsAtLevel(fstream* file, I3C_Frame* frame);
    int writePixels(fstream* file, I3C_Frame* frame);
    void writeParents(fstream* file, I3C_Frame* frame);


private:
    int m_i_sideSize;
    int m_i_numberOfLevels;
    int m_i_totalMaps;
    int m_i_buffer;
};

#endif // IMAGE_H
