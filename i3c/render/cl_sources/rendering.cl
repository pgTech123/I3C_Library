//Author:           Pascal Gendron
//Creation date:    October 1st 2015
//Description:      GPU basic functions for rendering



#define SCALE_FACTOR    1000    //Must fit SCALE_FACTOR in 'i3c_transform.h'
#define STACK_SIZE      88      //Enough for images size: 2048 x 2048 x 2048

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
    //Copy to local (limit access to global)
    float4 tmpCube3Pts[3];
    tmpCube3Pts[0] = cube3Pts[0];
    tmpCube3Pts[1] = cube3Pts[1];
    tmpCube3Pts[2] = cube3Pts[2];

    // Copy 3 points to cube 8 pts
    cube8Pts[0] = tmpCube3Pts[0].xyz;
    cube8Pts[2] = tmpCube3Pts[1].xyz;
    cube8Pts[3] = tmpCube3Pts[2].xyz;

    // 1 - Dispatch components
    cube8Pts[6].x = tmpCube3Pts[0].w;
    cube8Pts[6].y = tmpCube3Pts[1].w;
    cube8Pts[6].z = tmpCube3Pts[2].w;

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

bool lockIfNotAlready(__global int* childId_memStatusBit, int id)
{
    int unlockedValue = (childId_memStatusBit[id] & 0xBFFFFFFF);
    int lockedValue = (childId_memStatusBit[id] | 0x40000000);

    int old = atomic_cmpxchg(childId_memStatusBit+id, unlockedValue, lockedValue);
    if((old & 0x40000000) == 0){
        return true;    //Was not lock but is now
    }
    return false;       //Was already locked
}


//-----------------------------------------------------------------
//
//                        STACK FUNCTIONS
//
//--------------------------------------------------------------

void push(  __private char levelStackPtr[],
            __private int idStackPtr[],
            __private char* stackPtr,
            __private char level,
            __private int id)
{
    levelStackPtr[*stackPtr] = level;   //Push Level
    idStackPtr[*stackPtr] = id;         //Push Index
    (*stackPtr)++;
}

void pop(   __private char levelStackPtr[],
            __private int idStackPtr[],
            __private char* stackPtr,
            __private char* level,
            __private int* id)
{
    (*stackPtr)--;
    (*level) = levelStackPtr[*stackPtr];    //Pop Level
    (*id) = idStackPtr[*stackPtr];          //Pop Index
}

void orderedPush(   __private char stack_level[],
                    __private int stack_id[],
                    __private char* stackPtr,
                    __private char level,
                    __private int offsetFirstChild,
                    __private char map,
                    __global char* renderingOrder)
{
    int cubeNumber = offsetFirstChild;
    for(char i = 0; i < 8; i++){
        if(map & (0x01 << i)){
            push(stack_level, stack_id, stackPtr, level, offsetFirstChild + renderingOrder[cubeNumber]);
            cubeNumber ++;
        }
    }
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
void updateChildPosition( __global float4 *cubesStorage, __private int offsetFirstChild,
                          int offsetParent, char map, __global char* renderingOrder)
{
    int offsetParent_3 = 3 * (offsetParent & 0x3FFFFFFF);
    int offsetFirstChild_3 = 3*offsetFirstChild;
    //Used a lot so we prefer to have it local
    float4 cubeStorageOffsetParent3_1 = cubesStorage[offsetParent_3+1];

    // 1 - Get pt3
    float3 pt3;
    pt3.x = cubesStorage[offsetParent_3+0].w;
    pt3.y = cubeStorageOffsetParent3_1.w;
    pt3.z = cubesStorage[offsetParent_3+2].w;

    // 2 - Split arrays in 2
    float3 xVector, yVector, zVector;   // x,y,z -> cube axis
    xVector = (cubesStorage[offsetParent_3+2].xyz - cubeStorageOffsetParent3_1.xyz)*0.5;
    yVector = (cubesStorage[offsetParent_3+0].xyz - cubeStorageOffsetParent3_1.xyz)*0.5;
    zVector = (pt3 - cubeStorageOffsetParent3_1.xyz)*0.5;

    // 3 - Compute child
    float4 localResult[4];
    float unorderedDst[8];
    char cubeNumber = 0;
    const char xBool[8] = {1, 0, 0, 1, 1, 0, 0, 1};
    const char yBool[8] = {1, 1, 1, 1, 0, 0, 0, 0};
    const char zBool[8] = {1, 1, 0, 0, 1, 1, 0, 0};

    for(char i = 0; i < 8; i++){
        if(map & (0x01 << i)){
            char localOffset = 3*cubeNumber;

            float3 xVec = xVector*xBool[i];
            float3 yVec = yVector*yBool[i];
            float3 zVec = zVector*zBool[i];

            localResult[0].xyz = cubeStorageOffsetParent3_1.xyz + yVector + xVec + yVec + zVec;
            localResult[1].xyz = cubeStorageOffsetParent3_1.xyz +           xVec + yVec + zVec;
            localResult[2].xyz = cubeStorageOffsetParent3_1.xyz + xVector + xVec + yVec + zVec;
            localResult[3].xyz = cubeStorageOffsetParent3_1.xyz + zVector + xVec + yVec + zVec;

            localResult[0].w = localResult[3].x;
            localResult[1].w = localResult[3].y;
            localResult[2].w = localResult[3].z;

            cubesStorage[offsetFirstChild_3+ localOffset +0] = localResult[0];
            cubesStorage[offsetFirstChild_3+ localOffset +1] = localResult[1];
            cubesStorage[offsetFirstChild_3+ localOffset +2] = localResult[2];

            // Compute distance from viewer of child
            unorderedDst[cubeNumber] = fast_length((float3)localResult[0].xyz);
            cubeNumber++;
        }
    }

    float fartest = 0;
    float potentialNextFartest = 0;
    char potentialNextFartestIndex;

    // 4 - Order distance of child
    //
    // i = 0 => far
    // i -> 8 => close
    //
    for(int i = offsetFirstChild; i < offsetFirstChild+cubeNumber; i++){
        for(char j = 0; j < cubeNumber; j++){
            if(potentialNextFartest < unorderedDst[j]){
                potentialNextFartest = unorderedDst[j];
                potentialNextFartestIndex = j;
            }
        }

        fartest = unorderedDst[potentialNextFartestIndex];
        unorderedDst[potentialNextFartestIndex] = 0;    //Done with this one
        potentialNextFartest = 0;                       //reset for next round

        //5 - Store order in global memory
        renderingOrder[i] = potentialNextFartestIndex;
    }
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
        float scaleF_div_z = SCALE_FACTOR / fullCube[i].z;
        int x = (int)(fullCube[i].x * scaleF_div_z);
        int y = (int)(fullCube[i].y * scaleF_div_z);
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

void drawPixelCube(/*TODO*/)
{
    //TODO
}




//-----------------------------------------------------------------
//
//                             KERNEL
//
//-----------------------------------------------------------------

__kernel void render(   __write_only image2d_t texture,
                        __global int2 *screenOffset,
                        __global uchar4 *pixels,
                        __global uchar *cubeMap,
                        __global int *childId, // 2 MSB reserved for locks
                        __global float4 *cubeCorners,
                        __global uint *maxNumOfLevelPtr,
                        __global uint *topCubeIdPtr,
                        __write_only image2d_depth_t depthMapWrite,
                        __read_only image2d_depth_t depthMapRead,
                        __global char *renderingOrder)
{
    int4 boundingRect;
    int currentCubeId = topCubeIdPtr[0];
    char topLevel = maxNumOfLevelPtr[0]-1;

    int2 pixelCoord = (int2)( get_global_id(0) + screenOffset[0].x,
                              get_global_id(1) + screenOffset[0].y);

    //Stack parameters
    char stackPtr = 0;
    char stack_level[STACK_SIZE];
    int  stack_id[STACK_SIZE];

    for(char level = topLevel; level >= 0; level--){

        //TODO: rearrange code so that this is called only once per cube, not many time by every work items
        boundingRect = getPixelBoundingRect(cubeCorners, currentCubeId*3);

        //If not seen, pop next probable cube
        if(!isInBoundingRect(boundingRect, pixelCoord)){
            if(stackPtr <= 0){    return;    }  //Not seen
            pop(stack_level, stack_id, &stackPtr, &level, &currentCubeId);
            continue;
        }

        //If pixel cube, we draw it
        if(level == 0){ //Actually should be level < threshold
            int pixelId = childId[currentCubeId] & 0x3FFFFFFF;
            float4 pixelValue = (float4)((float)pixels[pixelId].x/255, (float)pixels[pixelId].y/255,
                                        (float)pixels[pixelId].z/255, 1.0 );
            write_imagef(texture, pixelCoord, pixelValue);
            write_imagef(depthMapWrite, pixelCoord, 0.7);
            return;
        }

        int offsetFirstChild = childId[currentCubeId] & 0x3FFFFFFF;
        char currentCubeMap = cubeMap[currentCubeId];

        if((childId[currentCubeId] & 0x80000000) == 0 && lockIfNotAlready(childId, currentCubeId))
        {
            updateChildPosition(cubeCorners, offsetFirstChild, currentCubeId, currentCubeMap, renderingOrder);
            atomic_or(childId+currentCubeId, 0x80000000);
        }
        while((childId[currentCubeId] & 0x80000000) == 0){} //Wait for the work to be done by someone else

        //Ordered push
        orderedPush(stack_level ,stack_id, &stackPtr, level, offsetFirstChild, currentCubeMap, renderingOrder);

        //Pop last and update IDs
        pop(stack_level, stack_id, &stackPtr, &level, &currentCubeId);

    }
}
