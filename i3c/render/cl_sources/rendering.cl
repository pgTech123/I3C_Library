//Author:           Pascal Gendron
//Creation date:    October 1st 2015
//Description:      GPU basic functions for rendering


//-----------------------------------------------------------------
//
//                  RECONSTRUCTION FUNCTIONS
//
//-----------------------------------------------------------------

// To save memory and memory access bandwidth, as well as some
// processing power, a cube is stored in memory as 3 points. The
// whole cube can then be reconstructed.

/*
 *     3 PTS CUBES                      8 PTS CUBES
 *     ------------                     ------------
 *
 *   (_pt 0)                              4 *------------------* 5
 *      *                                  /|                 /|
 *      |                                 / |                / |
 *      |                              0 *------------------* 1|
 *      |                                |  |               |  |           y
 *      |                                |  |               |  |           ^
 *      *------------*                   |  |               |  |           |  ^ z
 *   (_pt 1)       (_pt 2)               |  |               |  |           | /
 *                                       | 6*---------------|--* 7        -|-------> x
 *                                       | /                | /          (0,0,0)
 *                                       |/                 |/
 *                                     2 *------------------* 3
 */

void reconstructCube(/*__global*/float3* cube3Pts, float3* cube8Pts)
{
    // Copy 3 points to cube 8 pts
    cube8Pts[0] = cube3Pts[0];
    cube8Pts[2] = cube3Pts[1];
    cube8Pts[3] = cube3Pts[2];

    // 1 - Find distance between pt0 and pt1
    //float cubeLength = distance(cube3Pts[0], cube3Pts[1]);   //FIXME: For more optimisations, could use fast_distance()

    // 2 - Cross product to find pt6
    float4 vectPt1Pt0, vectPt1Pt2;
    vectPt1Pt0.xyz = cube3Pts[0] - cube3Pts[1];
    vectPt1Pt2.xyz = cube3Pts[2] - cube3Pts[1];

    //Actual cross product and adjust size
    //cube8Pts[6] = (normalize(cross(vectPt1Pt2, vectPt1Pt0).xyz)) * cubeLength;  //FIXME: maybe optimisation with sqrt because 90deg btw vectors
    cube8Pts[6] = sqrt(cross(vectPt1Pt2, vectPt1Pt0).xyz);

    // 3 - Find remaining points by copying translation of parallel vertice

    //Find point 4
    float3 translation = cube8Pts[6] - cube8Pts[2];
    cube8Pts[4] =  cube8Pts[0] + translation;

    //Find 3 remaining points (1, 5, 7)
    translation = cube8Pts[3] - cube8Pts[2];
    cube8Pts[1] =  cube8Pts[0] + translation;
    cube8Pts[5] =  cube8Pts[4] + translation;
    cube8Pts[7] =  cube8Pts[6] + translation;
}

/*
 *     3 PTS CUBES                      4 PTS FACE
 *     ------------                     ------------
 *
 *   (_pt 0)                      (_pt 0)       (_pt 3)
 *      *                             *-----------*
 *      |                             |           |
 *      |                             |           |
 *      |                             |           |
 *      |                             |           |
 *      *------------*                *-----------*
 *   (_pt 1)       (_pt 2)         (_pt 1)       (_pt 2)
 */

void reconstructFace(float3* cube3Pts, float3* face4Pts)
{
    face4Pts[0] = cube3Pts[0];
    face4Pts[1] = cube3Pts[1];
    face4Pts[2] = cube3Pts[2];

    //Find the remaining point by copying translation of parallel vertice
    float3 translation = cube3Pts[2] - cube3Pts[1];
    face4Pts[3] = cube3Pts[0] + translation;
}

float3 getFaceMissingPoint(float3* cube3Pts)
{
    //Find the remaining point by copying translation of parallel vertice
    float3 translation = cube3Pts[2] - cube3Pts[1];
    return (cube3Pts[0] + translation);
}

/*
 *     3 PTS CUBE                       4 PTS CORNER
 *     -----------                      -------------
 *
 *   (_pt 0)                      (_pt 0)
 *      *                             *
 *      |                             |
 *      |                             |  * (_pt 3)
 *      |                             | /
 *      |                             |/
 *      *------------*                *-----------*
 *   (_pt 1)       (_pt 2)         (_pt 1)       (_pt 2)
 */
void reconstructCorner(float3* cube3Pts, float3* corner4Pts)
{
    // 1 - Copy 3 points to cube 8 pts
    corner4Pts[0] = cube3Pts[0];
    corner4Pts[1] = cube3Pts[1];
    corner4Pts[2] = cube3Pts[2];

    // 2 - Cross product to find pt3
    float4 vectPt1Pt0, vectPt1Pt2;
    vectPt1Pt0.xyz = cube3Pts[0] - cube3Pts[1];
    vectPt1Pt2.xyz = cube3Pts[2] - cube3Pts[1];

    // 3 - Actual cross product and adjust size
    corner4Pts[3] = sqrt(cross(vectPt1Pt2, vectPt1Pt0).xyz);
}

float3 getCornerMissingPoint(float3* cube3Pts)
{
    // 1 - Cross product to find pt3
    float4 vectPt1Pt0, vectPt1Pt2;
    vectPt1Pt0.xyz = cube3Pts[0] - cube3Pts[1];
    vectPt1Pt2.xyz = cube3Pts[2] - cube3Pts[1];

    // 2 - Actual cross product and adjust size
    return sqrt(cross(vectPt1Pt2, vectPt1Pt0).xyz);
}



//-----------------------------------------------------------------
//
//                CHILDREN COMPUTATION FUNCTIONS
//
//-----------------------------------------------------------------

void updateChildPosition( __global float3 *cubesStorage, int offsetParent,
                          int offsetFirstChild, char map)
{
    //TODO: Mark cube as computing in progress
    //TODO: Compute child
    //TODO: Atomic write & make child cubes available
}

int4 getPixelBoundingRect(__global float3 *cubesStorage, int offset)
{
    //TODO: (basically function project to clean)
}






// Compute child cubes
void computeChildCubes(__global float3 *parentCube, float3 *childCube)
{
/* reconstruct face, find mid, store child*/
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
