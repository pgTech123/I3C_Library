//Author:           Pascal Gendron
//Creation date:    October 1st 2015
//Description:


__kernel void render(   __write_only image2d_t texture,
                        __global int2 *screenOffset /*,
                        __global uchar3 *pixels,
                        __global uchar *cubesMap,
                        __global int *childId*/)
{
    int2 pixelCoord = (int2)( get_global_id(0) + screenOffset[0].x,
                              get_global_id(1) + screenOffset[0].y );

    //TODO
    //DEBUG:
    float4 pixelValue = (float4)(1.0, 1.0, 0.5, 1.0);
    write_imagef(texture, pixelCoord, pixelValue);
}


__kernel void clearTexture(__write_only image2d_t texture)
{
    int2 pixelCoord = (int2)( get_global_id(0), get_global_id(1));
    float4 pixelValue = (float4)(0.0, 0.0, 0.0, 1.0);
    write_imagef(texture, pixelCoord, pixelValue);
}
