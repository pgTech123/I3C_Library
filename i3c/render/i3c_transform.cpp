#include "i3c_transform.h"
#include "math.h"

void matFromPosAndRot(Mat4x4* transformMatrix, Position position, Axis rotation)
{
    //Precompute sin and cos
    float cosAngleX = cos(rotation.x);
    float sinAngleX = sin(rotation.x);
    float cosAngleY = cos(rotation.y);
    float sinAngleY = sin(rotation.y);
    float cosAngleZ = cos(rotation.z);
    float sinAngleZ = sin(rotation.z);

    float cosXSinY = cosAngleX * sinAngleY;
    float sinXSinY = sinAngleX * sinAngleY;

    transformMatrix->element[0][0] = cosAngleY * cosAngleZ;
    transformMatrix->element[0][1] = -cosAngleY * sinAngleZ;
    transformMatrix->element[0][2] = sinAngleY;
    transformMatrix->element[0][3] = position.x;

    transformMatrix->element[1][0] = sinXSinY * cosAngleZ + cosAngleX * sinAngleZ;
    transformMatrix->element[1][1] = - sinXSinY * sinAngleZ + cosAngleX * cosAngleZ;
    transformMatrix->element[1][2] = - sinAngleX * cosAngleY;
    transformMatrix->element[1][3] = position.y;

    transformMatrix->element[2][0] = - cosXSinY * cosAngleZ + sinAngleX * sinAngleZ;
    transformMatrix->element[2][1] = cosXSinY * sinAngleZ + sinAngleX * cosAngleZ;
    transformMatrix->element[2][2] = cosAngleX * cosAngleY;
    transformMatrix->element[2][3] = position.z;

    transformMatrix->element[3][0] = 0;
    transformMatrix->element[3][1] = 0;
    transformMatrix->element[3][2] = 0;
    transformMatrix->element[3][3] = 1;
}

void matFromPosAndRot(Mat4x4* transformMatrix, Position position, Axis rotation, Axis scale)
{
    matFromPosAndRot(transformMatrix, position, rotation);
    //TODO: !!!!!! Adjust scale !!!!!!
}

void computeTransform(Mat4x4* originalCoord,
                      ObjectBoundaries* transformedObject,
                      Mat4x4* transformMatrix)
{
    Mat4x4 transformSumMatrix;

    //Multiply originalCoord and transformMatrix
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            transformSumMatrix.element[i][j] = transformMatrix->element[i][0]*originalCoord->element[0][j]+
                                               transformMatrix->element[i][1]*originalCoord->element[1][j]+
                                               transformMatrix->element[i][2]*originalCoord->element[2][j]+
                                               transformMatrix->element[i][3]*originalCoord->element[3][j];

        }
    }

    //Apply the transform matrix to default points
    for(int i=0; i < 8; i++){
        //Transform DefaultCube and copy to transformed object
        transformedObject->x[i] =(DefaultCube.x[i]*transformSumMatrix.element[0][0]+
                                  DefaultCube.y[i]*transformSumMatrix.element[0][1]+
                                  DefaultCube.z[i]*transformSumMatrix.element[0][2]+
                                  /* (1x) */       transformSumMatrix.element[0][3]);

        transformedObject->y[i] =(DefaultCube.x[i]*transformSumMatrix.element[1][0]+
                                  DefaultCube.y[i]*transformSumMatrix.element[1][1]+
                                  DefaultCube.z[i]*transformSumMatrix.element[1][2]+
                                  /* (1x) */       transformSumMatrix.element[1][3]);

        transformedObject->z[i] =(DefaultCube.x[i]*transformSumMatrix.element[2][0]+
                                  DefaultCube.y[i]*transformSumMatrix.element[2][1]+
                                  DefaultCube.z[i]*transformSumMatrix.element[2][2]+
                                  /* (1x) */       transformSumMatrix.element[2][3]);

        /*cout << "X: " << transformedObject->x[i] << endl;
        cout << "Y: " << transformedObject->y[i] << endl;
        cout << "Z: " << transformedObject->z[i] << endl;*/
    }
}

void projectObject(ObjectBoundaries* transformedObject, ScreenBoundaries* screenBound, int screenW, int screenH)
{
    int lowX = screenW;
    int highX = 0;
    int lowY = screenH;
    int highY = 0;

    for(int i = 0; i < 8; i++){
        if(transformedObject->z[i] <= 0){
            //Set screen boundaries
            screenBound->x = 0;
            screenBound->y = 0;
            screenBound->w = 0;
            screenBound->h = 0;
        }

        float tmpX = transformedObject->x[i] * SCALE_FACTOR/transformedObject->z[i];
        float tmpY = transformedObject->y[i] * SCALE_FACTOR/transformedObject->z[i];

        /*cout << "TMPX = " << tmpX << endl;
        cout << "TMPY = " << tmpY << endl;*/

        if(tmpX < lowX){
            lowX = tmpX;
        }
        if(tmpX > highX){
            highX = tmpX;
        }
        if(tmpY < lowY){
            lowY = tmpY;
        }
        if(tmpY > highY){
            highY = tmpY;
        }
        if(lowX < 0){
            lowX = 0;
        }
        if(lowY < 0){
            lowY = 0;
        }
        if(highX >= screenW){
            highX = screenW;
        }
        if(highY >= screenH){
            highY = screenH;
        }
    }

    int w = highX-lowX;
    int h = highY - lowY;
    if(w < 0){
        w = 0;
    }
    if(h < 0){
        h = 0;
    }

    //DEBUG
    /*cout << "LowX = " << lowX << endl;
    cout << "LowY = " << lowY << endl;
    cout << "w = " << w << endl;
    cout << "h = " << h << endl;*/


    //Set screen boundaries
    screenBound->x = lowX;
    screenBound->y = lowY;
    screenBound->w = w;
    screenBound->h = h;
}
