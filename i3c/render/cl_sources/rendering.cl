//Author:           Pascal Gendron
//Creation date:    October 1st 2015
//Description:      GPU basic functions for rendering



#define SCALE_FACTOR    1000    //Must fit SCALE_FACTOR in 'i3c_transform.h'


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

void reconstructCube3(__global float3* cube3Pts, float3* cube8Pts)
{
    // Copy 3 points to cube 8 pts
    cube8Pts[0] = cube3Pts[0];
    cube8Pts[2] = cube3Pts[1];
    cube8Pts[3] = cube3Pts[2];

    // 1 - Cross product to find pt6
    float3 vectPt1Pt0, vectPt1Pt2;
    vectPt1Pt0 = cube3Pts[0] - cube3Pts[1];
    vectPt1Pt2 = cube3Pts[2] - cube3Pts[1];

    //Actual cross product and adjust size
    cube8Pts[6] = cube3Pts[1] + cross(vectPt1Pt2, vectPt1Pt0).xyz;

    // 2 - Find remaining points by copying translation of parallel vertice
    float3 translation = cube8Pts[6] - cube8Pts[2];
    cube8Pts[4] =  cube3Pts[0] + translation;

    //Find 3 remaining points (1, 5, 7)
    translation = cube3Pts[2] - cube3Pts[1];
    cube8Pts[1] = cube8Pts[0] + translation;
    cube8Pts[5] = cube8Pts[4] + translation;
    cube8Pts[7] = cube8Pts[6] + translation;
}


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

void reconstructCube4(__global float4 cube3Pts[3], float3 cube8Pts[8])
{
    // Copy 3 points to cube 8 pts
    cube8Pts[0] = cube3Pts[0].xyz;
    cube8Pts[2] = cube3Pts[1].xyz;
    cube8Pts[3] = cube3Pts[2].xyz;

    // 1 - Dispatch components
    cube8Pts[6].x = cube3Pts[0].w;
    cube8Pts[6].y = cube3Pts[1].w;
    cube8Pts[6].z = cube3Pts[2].w;

    // 2 - Find remaining points by copying translation of parallel vertice
    float3 translation = cube8Pts[6] - cube8Pts[2];
    cube8Pts[4] =  cube8Pts[0] + translation;

    //Find 3 remaining points (1, 5, 7)
    translation = cube8Pts[3] - cube8Pts[2];
    cube8Pts[1] = cube8Pts[0] + translation;
    cube8Pts[5] = cube8Pts[4] + translation;
    cube8Pts[7] = cube8Pts[6] + translation;
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

void reconstructFace3(__global float3* cube3Pts, float3* face4Pts)
{
    face4Pts[0] = cube3Pts[0];
    face4Pts[1] = cube3Pts[1];
    face4Pts[2] = cube3Pts[2];

    //Find the remaining point by copying translation of parallel vertice
    float3 translation = cube3Pts[2] - cube3Pts[1];
    face4Pts[3] = cube3Pts[0] + translation;
}

float3 getFaceMissingPoint3(__global float3* cube3Pts)
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
void reconstructCorner3(__global float3* cube3Pts, float3* corner4Pts)
{
    // 1 - Copy 3 points to cube 8 pts
    corner4Pts[0] = cube3Pts[0];
    corner4Pts[1] = cube3Pts[1];
    corner4Pts[2] = cube3Pts[2];

    // 2 - Cross product to find pt3
    float3 vectPt1Pt0, vectPt1Pt2;
    vectPt1Pt0.xyz = cube3Pts[0] - cube3Pts[1];
    vectPt1Pt2.xyz = cube3Pts[2] - cube3Pts[1];

    // 3 - Actual cross product and adjust size
    corner4Pts[3] = cube3Pts[1] + cross(vectPt1Pt2, vectPt1Pt0);
}

float3 getCornerMissingPoint3(__global float3* cube3Pts)
{
    // 1 - Cross product to find pt3
    float3 vectPt1Pt0, vectPt1Pt2;
    vectPt1Pt0 = cube3Pts[0] - cube3Pts[1];
    vectPt1Pt2 = cube3Pts[2] - cube3Pts[1];

    return cube3Pts[1] + cross(vectPt1Pt2, vectPt1Pt0);
}

float3 getCornerMissingPointDirection3(__global float3* cube3Pts)
{
    // 1 - Cross product to find pt3
    float3 vectPt1Pt0, vectPt1Pt2;
    vectPt1Pt0 = cube3Pts[0] - cube3Pts[1];
    vectPt1Pt2 = cube3Pts[2] - cube3Pts[1];

    return cross(vectPt1Pt2, vectPt1Pt0);
}


//-----------------------------------------------------------------
//
//                    MEMORY LOCK FUNCTIONS
//
//-----------------------------------------------------------------

bool lockIfNotAlready(__global uint* memoryWithLockBit, int index)
{
    int unlockedValue = (memoryWithLockBit[index] & 0xBFFFFFFF);
    int lockedValue   = (memoryWithLockBit[index] | 0x40000000);
    int old = atomic_cmpxchg(memoryWithLockBit+index, unlockedValue, lockedValue);
    if((old & 0x40000000) == 0)
    {
        return true;    //Is now locked
    }
    return false;       //Was already locked
}


//-----------------------------------------------------------------
//
//                CHILDREN COMPUTATION FUNCTIONS
//
//-----------------------------------------------------------------
/*
 *                 4 PTS CORNER
 *                --------------
 *
 *            (_pt 0)
 *               *
 *               |                            y
 *               |  * (_pt 3)                 ^
 *               | /                          |
 *               |/                           |  ^ z
 *               *-----------*                | /
 *            (_pt 1)       (_pt 2)          -|-------> x
 *
 */
void updateChildPosition( __global float4 *cubesStorage, __global uint *memoryLock, int offsetParent,
                          int offsetFirstChild, char map)
{
    //If was already lock, someone else is computing it so we wait for the result
    if(!lockIfNotAlready(memoryLock, offsetParent)){
        //while((memoryLock[offsetParent] & 0x80000000) == 0){}
        //return;
    }

    // 1 - Get pt3
    float3 pt3;
    float3 newPt3;
    pt3.x = cubesStorage[offsetParent+0].w;
    pt3.y = cubesStorage[offsetParent+1].w;
    pt3.z = cubesStorage[offsetParent+2].w;

    // 2 - Split arrays in 2
    float3 xVector, yVector, zVector;   // x,y,z -> cube axis
    xVector = (cubesStorage[offsetParent+2].xyz - cubesStorage[offsetParent+1].xyz)/2;
    yVector = (cubesStorage[offsetParent+0].xyz - cubesStorage[offsetParent+1].xyz)/2;
    zVector = (pt3 - cubesStorage[offsetParent+1].xyz)/2;

    // 3 - According to map, write child
    const char xBool[8] = {0, 1, 0, 1, 0, 1, 0, 1};
    const char yBool[8] = {1, 1, 0, 0, 1, 1, 0, 0};
    const char zBool[8] = {0, 0, 0, 0, 1, 1, 1, 1};
    for(char i = 0; i < 8; i++){
        if(map & (0x01 << i)){
        //FIXME: Optimisation to do here...
            char localOffset = 3*i;
            cubesStorage[offsetFirstChild+ localOffset +0].xyz = cubesStorage[offsetParent+1].xyz + yVector + xVector*xBool[i] + yVector*yBool[i] + zVector*zBool[i];
            cubesStorage[offsetFirstChild+ localOffset +1].xyz = cubesStorage[offsetParent+1].xyz +           xVector*xBool[i] + yVector*yBool[i] + zVector*zBool[i];
            cubesStorage[offsetFirstChild+ localOffset +2].xyz = cubesStorage[offsetParent+1].xyz + xVector + xVector*xBool[i] + yVector*yBool[i] + zVector*zBool[i];
            newPt3 = cubesStorage[offsetParent+1].xyz + zVector + xVector*xBool[i] + yVector*yBool[i] + zVector*zBool[i];

            cubesStorage[offsetFirstChild+ localOffset +0].w = newPt3.x;
            cubesStorage[offsetFirstChild+ localOffset +1].w = newPt3.y;
            cubesStorage[offsetFirstChild+ localOffset +2].w = newPt3.z;
        }
    }

    // 4 - Atomic write data ready
    atomic_or(memoryLock+offsetParent, 0x80000000);
}

int4 getPixelBoundingRect(__global float4 *cubesStorage, int offset)
{
    int4 boundingRect;
    int minx = 100000;
    int maxx = 0;
    int miny = 100000;
    int maxy = 0;
    float3 fullCube[8];

    // 1 - Reconstruct full cube
    reconstructCube4(cubesStorage+offset, fullCube);

    // 2 - Find bounding rect
    for(uchar i = 0; i < 8; i++){
        if(fullCube[i].z <= 0){    continue;    }   //Not seen
        int x = (int)(fullCube[i].x * SCALE_FACTOR / fullCube[i].z);
        int y = (int)(fullCube[i].y * SCALE_FACTOR / fullCube[i].z);
        if(x < minx){   minx = x;   }
        if(x > maxx){   maxx = x;   }
        if(y < miny){   miny = y;   }
        if(y > maxy){   maxy = y;   }
    }

    // 3 - Write bounding rect
    boundingRect.x = minx;
    boundingRect.y = maxx;
    boundingRect.z = miny;
    boundingRect.w = maxy;

    return boundingRect;
}


//-----------------------------------------------------------------
//
//                        IMAGE FUNCTIONS
//
//-----------------------------------------------------------------

bool isInBoundingRect(const int4 boundingRect, const int2 imgCoord)
{
    if( boundingRect.x <= imgCoord.x &&
        boundingRect.y >  imgCoord.x &&
        boundingRect.z <= imgCoord.y &&
        boundingRect.w >  imgCoord.y )
    {
        return true;
    }
    else{
        return false;
    }
}


//-----------------------------------------------------------------
//
//                        PIXELS FUNCTIONS
//
//-----------------------------------------------------------------
//TODO: char4 HSV2RGB()



//-----------------------------------------------------------------
//
//                             KERNEL
//
//-----------------------------------------------------------------

__kernel void render(   __write_only image2d_t texture,
                        __global int2 *screenOffset,
                        __global uchar4 *pixels,
                        __global uchar *cubeMap,
                        __global uint *childId, // 2 MSB reserved for locks
                        __global float4 *cubeCorners,
                        __global uint *maxNumOfLevelPtr)
{
    int currentCubeId = 0;       //Start by the beginning
    int childCubeId = childId[currentCubeId] & 0x3FFFFFFF;  //Remove status bits
    int4 boundingRect;
    uint maxNumOfLevel = maxNumOfLevelPtr[0];
    int2 pixelCoord = (int2)( get_global_id(0) + screenOffset[0].x,
                              get_global_id(1) + screenOffset[0].y);

    //DEBUG: Work item size
    if(get_global_id(0) %2){
        float4 pixelValue = (float4)(1.0, 0.0, 0.0, 1.0 );
        write_imagef(texture, pixelCoord, pixelValue);
    }

    //Actual magic happens here
    for(int i = 0; i < maxNumOfLevel; i++){
        boundingRect = getPixelBoundingRect(&cubeCorners[currentCubeId], 0);

        //TODO: push on stack depending on dst
        if(!isInBoundingRect(boundingRect, pixelCoord) ){
            //TODO: COMPUTE SOMETHING ELSE
            return; //Temporary
        }

        updateChildPosition(cubeCorners, childId, currentCubeId, 3 * childCubeId, 255/*cubeMap[currentCubeId]*/);
        //TODO: update current cube ID and child ID
    }

    for(int i = 1; i < 9; i++){
        boundingRect = getPixelBoundingRect(&cubeCorners[i*3], 0);
        if(isInBoundingRect(boundingRect, pixelCoord) ){
            //float4 pixelValue = (float4)(1.0, 5.0, 0.5, 1.0);
            int tmp= 0;
            float4 pixelValue = (float4)(1.0/i, 1.0/i, 1.0/i, 1.0 );
            /*float4 pixelValue = (float4)((float)pixels[tmp].x/255, (float)pixels[tmp].y/255,
                                         (float)pixels[tmp].z/255, 1.0 );*/
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
