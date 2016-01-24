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

using namespace std;

#ifndef __YUV_CONVERSION
#define __YUV_CONVERSION
//source: http://stackoverflow.com/questions/1737726/how-to-perform-rgb-yuv-conversion-in-c-c
#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)
//end source
#endif


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

    void writeHeader(fstream* file);
    void writeResolution(fstream* file, I3C_Frame* frame);
    void writeMapAtLevel(fstream* file, I3C_Frame* frame);
};

#endif // IMAGEV2_H
