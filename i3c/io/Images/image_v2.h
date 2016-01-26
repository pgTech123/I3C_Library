//Author:           Pascal Gendron
//Creation date:    January 23th 2016
//Description:      Image file V2 (read and write functions)

#ifndef IMAGEV2_H
#define IMAGEV2_H

#include <fstream>
#include <iostream>
#include <string>

#include "imagefile.h"
#include "../../i3c.h"
#include "../../utils/binary_func.h"
#include "../../utils/i3c_converter.h"

using namespace std;


class ImageV2
{
public:
    ImageV2();
    ~ImageV2();

    int read(fstream* file, I3C_Frame* frame);
    int write(fstream* file, I3C_Frame* frame);

private:
    int readResolution(fstream* file, I3C_Frame* frame);
    int readMapAtLevel(fstream* file, I3C_Frame* frame);
    unsigned int readNumOfPixel(fstream* file, I3C_Frame* frame);
    //void readAverageYUV(fstream* file, I3C_Frame* frame);

    void writeHeader(fstream* file);
    void writeResolution(fstream* file, I3C_Frame* frame);
    void writeMapAtLevel(fstream* file, I3C_Frame* frame);
    void writeAverageYUV(fstream* file, I3C_Frame* frame);

    void convertRGB2YUV(I3C_Frame* frame, YUV* yuvPixels);
};

#endif // IMAGEV2_H
