#ifndef I3C_EDITINGCUBE_H
#define I3C_EDITINGCUBE_H

//#ifdef I3C_EDITING_MODULE_ENABLED

#include "i3c.h"
#include <iostream>
using namespace std;

#define RECENTER(A, W)  ((A) > (W) ? (A-W) : (A))

class I3C_EditingCube
{
public:
    I3C_EditingCube(int width);
    ~I3C_EditingCube();

    void addCube(int x, int y, int z, Pixel pixel);
    void removeCube(int x, int y, int z);
    void getPixelAt(int x, int y, int z, Pixel* pixel);

    void propageteAverage();
    unsigned char getMap();
    Pixel getAverage();

private:
    unsigned char cubeId(int* x, int* y, int* z);
    void deleteChild(unsigned char cubeId);

private:
    int m_width;  //in pixels
    unsigned char m_map;
    I3C_EditingCube* m_childCube[8];

    Pixel m_avgPixel;
    bool m_avgPxIsSet;
};
//#endif
#endif // I3C_EDITINGCUBE_H
