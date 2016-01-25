#include "i3c_converter.h"

I3C_Converter::I3C_Converter()
{

}

void I3C_Converter::Pixel2YUV(Pixel *pixel, YUV *yuv)
{
    yuv->Y = RGB2Y(pixel->red, pixel->green, pixel->blue);
    yuv->U = RGB2U(pixel->red, pixel->green, pixel->blue);
    yuv->V = RGB2V(pixel->red, pixel->green, pixel->blue);
}

void I3C_Converter::YUV2Pixel(YUV *yuv, Pixel *pixel)
{
    pixel->red = YUV2R(yuv->Y, yuv->U, yuv->V);
    pixel->green = YUV2G(yuv->Y, yuv->U, yuv->V);
    pixel->blue = YUV2B(yuv->Y, yuv->U, yuv->V);
}

void I3C_Converter::getAverageYUVfromPixels(Pixel *pixelArray, int arraySize, YUV *yuv_out)
{
    YUV tmp;
    float y = 0;
    float u = 0;
    float v = 0;

    for(int i = 0; i < arraySize; i++){
        Pixel2YUV(&pixelArray[i], &tmp);
        y += (float)tmp.Y;
        u += (float)tmp.U;
        v += (float)tmp.V;
    }
    yuv_out->Y = (char)(y/arraySize);
    yuv_out->U = (char)(u/arraySize);
    yuv_out->V = (char)(v/arraySize);
}
