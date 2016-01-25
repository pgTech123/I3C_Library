//Author:           Pascal Gendron
//Creation date:    January 24th 2016
//Description:      Converter between different representations

#ifndef I3C_CONVERTER_H
#define I3C_CONVERTER_H

#include <iostream>

#include "../i3c.h"

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

class I3C_Converter
{
public:
    I3C_Converter();

    static void Pixel2YUV(Pixel *pixel, YUV *yuv);
    static void YUV2Pixel(YUV *yuv, Pixel *pixel);

    static void getAverageYUVfromPixels(Pixel *pixelArray, int arraySize, YUV *yuv_out);
};

#endif // I3C_CONVERTER_H
