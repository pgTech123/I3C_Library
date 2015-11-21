//Author:           Pascal Gendron
//Creation date:    October 1st 2015
//Description:      GPU basic functions for rendering


// Compute child cubes
void computeChildCubes(__global float3 *parentCube, float3 *childCube)
{
    float3 childCorners[27];

    //Order of computation: see diagram cause without it, this is really hard to understand
    uchar childBaseCorner[8] = {0, 1, 10, 9, 3, 4, 13, 12};     //Order important
    uchar outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
    uchar midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    uchar computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                                {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
    uchar midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
    uchar computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};

    //Subcorners computation
    for(uint i = 0; i < 8; i++){
        childCorners[outterCornersIndex[i]] = parentCube[i];
    }
    for(uchar i = 0; i < 12; i++){
        childCorners[midCornersIndex[i]] = (childCorners[computeMidWith[i][0]] + childCorners[computeMidWith[i][1]])/2.0;
    }
    for(uchar i = 0; i < 6; i++){
        childCorners[midFacesIndex[i]] = (childCorners[computeMidFaceWith[i][0]] + childCorners[computeMidFaceWith[i][1]])/2.0;
    }
    childCorners[13] = (childCorners[10] + childCorners[16])/2.0;

    //Fill childs
    for(uchar i = 0; i < 8; i++){
        for(uchar corner = 0; corner < 8; corner++){
            //Write child corner
            childCube[i*8+corner] = childCorners[childBaseCorner[i]+childBaseCorner[corner]];
        }
    }
}

int4 project(float3 *cube)
{
    int4 boundingRect = (int4)(0, 500, 0, 500);

    int minx = 100000;
    int maxx = 0;
    int miny = 100000;
    int maxy = 0;

    //Compute bounding rect
    for(uchar i = 0; i < 8; i++)
    {
        int x = (int)(cube[i].x / cube[i].z);
        int y = (int)(cube[i].y / cube[i].z);

        //Bounding rect test
        if(x < minx){
            minx = x;
        }
        if(x > maxx){
            maxx = x;
        }
        if(y < miny){
            miny = y;
        }
        if(y > maxy){
            maxy = y;
        }
    }

    //Write bounding rect
    boundingRect.x = minx;
    boundingRect.y = maxx;
    boundingRect.z = miny;
    boundingRect.w = maxy;

    return boundingRect;
}

// Checks if the pixel we're working on is in the bounding rect
bool isInBoundingRect(const int4 boundingRect, const int2 imgCoord)
{
    if( boundingRect.x <= imgCoord.x &&
        boundingRect.y > imgCoord.x &&
        boundingRect.z <= imgCoord.y &&
        boundingRect.w > imgCoord.y )
    {
        return true;
    }
    else{
        return false;
    }
}


// Render Function
__kernel void render(   __write_only image2d_t texture,
                        __global int2 *screenOffset,
                        __global uchar3 *pixels,
                        __global uchar *cubesMap,
                        __global int *childId,
                        __global float3 *cubeCorners)
{
    int2 pixelCoord = (int2)( get_global_id(0) + screenOffset[0].x,
                              get_global_id(1) + screenOffset[0].y );


    float3 childCorners[8*8];
    computeChildCubes(cubeCorners, childCorners);

    for(int i = 0; i < 8; i++){
        int4 boundingRect = project(&childCorners[i*8]);

        // If pixel is seen, draw it.
        if(isInBoundingRect(boundingRect, pixelCoord) /* and depth ok */){
            float4 pixelValue = (float4)(1.0/i, 5.0, 0.5, 1.0);
            write_imagef(texture, pixelCoord, pixelValue);
            return;
        }
    }
}


// CLEARS THE TEXTURE
__kernel void clearTexture(__write_only image2d_t texture)
{
    int2 pixelCoord = (int2)( get_global_id(0), get_global_id(1));
    float4 pixelValue = (float4)(0.0, 0.0, 0.0, 1.0);
    write_imagef(texture, pixelCoord, pixelValue);
}
