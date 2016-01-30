//Author:           Pascal Gendron
//Creation date:    November 2nd 2015
//Description:      Procedural Utils to compute
//                  transformations. Default Object
//                  stands in a cube of 1x1x1.

#ifndef I3C_TRANSFORM_H
#define I3C_TRANSFORM_H

#ifdef I3C_RENDER_MODULE_ENABLED

#include <iostream>
using namespace std;


#define SCALE_FACTOR        1000

struct Axis{
    float x;
    float y;
    float z;
};

//Create a type position and scale (x,y,z)
typedef Axis Position;

struct ObjectCoordinates{
    Position position;
    Axis rotation;
    Axis scale;
};

struct ObjectBoundaries{
    float x[8];
    float y[8];
    float z[8];
};

struct ScreenBoundaries{
    int x;
    int y;
    int w;
    int h;
};

struct Mat4x4{
    float element[4][4];
};


/*  DEFAULT CUBE:
 *
 *         4 *------------------* 5
 *          /|                 /|
 *         / |                / |
 *      0 *------------------* 1|
 *        |  |               |  |           y
 *        |  |               |  |           ^
 *        |  |               |  |           |  ^ z
 *        |  |               |  |           | /
 *        | 6*---------------|--* 7        -|-------> x
 *        | /                | /          (0,0,0)
 *        |/                 |/
 *      2 *------------------* 3
 *
 */

//             Default Cube Corner:  [ 0, 1, 2, 3, 4, 5, 6, 7 ]
const ObjectBoundaries DefaultCube = {{0, 1, 0, 1, 0, 1, 0, 1}, // X
                                      {1, 1, 0, 0, 1, 1, 0, 0}, // Y
                                      {0, 0, 0, 0, 1, 1, 1, 1}};// Z


/*#ifdef __cplusplus
extern "C" {
#endif*/

void matFromPosAndRot(Mat4x4* transformMatrix, Position position, Axis rotation);
void matFromPosAndRot(Mat4x4* transformMatrix, Position position, Axis rotation, Axis scale);

void computeTransform(Mat4x4* originalCoord,
                      ObjectBoundaries* transformedObject,
                      Mat4x4* transformMatrix);

void projectObject(ObjectBoundaries* originalCoord, ScreenBoundaries* screenBound, int screenW, int screenH);

/*#ifdef __cplusplus
}
#endif*/

#endif

#endif // I3C_TRANSFORM_H
