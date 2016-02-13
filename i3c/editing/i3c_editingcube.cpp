//#ifdef I3C_EDITING_MODULE_ENABLED

#include "i3c_editingcube.h"

I3C_EditingCube::I3C_EditingCube(int width)
{
    m_width = width;
    m_avgPxIsSet = false;

    for(int i = 0; i < 8; i++){
        m_childCube[i] = NULL;
    }
    m_map = 0;
}

I3C_EditingCube::~I3C_EditingCube()
{
    for(int i = 0; i < 8; i++){
        if(m_childCube[i] != NULL){
            delete m_childCube[i];
        }
    }
    m_map = 0;
}

void I3C_EditingCube::addCube(int x, int y, int z, Pixel pixel)
{
    if(m_width == 1){
        m_avgPixel = pixel;
    }
    else{
        m_avgPxIsSet = false;
        unsigned char cube = this->cubeId(&x, &y, &z);

        if((m_map & (0x01 << cube)) == 0){
            //Child cube did not exist so we create it
            m_childCube[cube] = new I3C_EditingCube(m_width/2);

            //Update map
            m_map |= (0x01 << cube);
        }
        //Call addCube on child
        m_childCube[cube]->addCube(x, y, z, pixel);
    }
}

void I3C_EditingCube::removeCube(int x, int y, int z)
{
    unsigned char cube = this->cubeId(&x, &y, &z);

    // Make sure that there is something there
    if(m_childCube[cube] != NULL){

        // If child is a pixel, delete it
        if(m_width == 2){
            this->deleteChild(cube);
            return;
        }

        // If child is not a pixel, call remove on child
        m_childCube[cube]->removeCube(x, y, z);

        // If child empty, delete child
        if(m_childCube[cube]->getMap() == 0){
            this->deleteChild(cube);
            return;
        }
    }
}

void I3C_EditingCube::getPixelAt(int x, int y, int z, Pixel* pixel)
{
    if(m_width == 1){
        *pixel = m_avgPixel;
        return;
    }

    unsigned char cube = this->cubeId(&x, &y, &z);
    if(m_childCube[cube] != NULL){
        m_childCube[cube]->getPixelAt(x, y, z, pixel);
        return;
    }
    (*pixel).red = 0;
    (*pixel).green = 0;
    (*pixel).blue = 0;
    return;
}

void I3C_EditingCube::setCubes(Pixel* pixel, unsigned char* cubeMap, unsigned int* childCubeId, int myId)
{
    if(m_width == 1){
        //Pixel child
        m_avgPixel = pixel[myId];
        m_avgPxIsSet = true;
        return;
    }
    for(int i = 0; i < 8; i++){
        if((cubeMap[myId] & (0x01 << i)) != 0){
            if(m_map &= (0x01 << i) != 0){
                delete m_childCube[i];
            }
            m_map |= (0x01 << i);
            m_childCube[i] = new I3C_EditingCube(m_width/2);
            m_childCube[i]->setCubes(pixel, cubeMap, childCubeId, childCubeId[myId+i]);
        }
    }
}

void I3C_EditingCube::propageteAverage()
{
    if(m_width > 2){
        propageteAverage();
    }
    int sum = 0;
    int redTot = 0, greenTot = 0, blueTot = 0;

    //Compute average
    for(int i = 0; i < 8; i++){
        if((m_map & (0x01 << i)) != 0){
            sum++;
            Pixel pix = m_childCube[i]->getAverage();
            redTot += pix.red;
            greenTot += pix.green;
            blueTot +=pix.blue;
        }
    }
    //Update average
    m_avgPixel.red = redTot/sum;
    m_avgPixel.green = greenTot/sum;
    m_avgPixel.blue = blueTot/sum;
    m_avgPxIsSet = true;
}

unsigned char I3C_EditingCube::getMap()
{
    return m_map;
}

Pixel I3C_EditingCube::getAverage()
{
    return m_avgPixel;
}

int I3C_EditingCube::getChildCount()
{
    if(m_width > 1){
        int count = 0;
        for(int i = 0; i < 8; i++){
            if((m_map& (0x01 << i)) != 0){
                count += m_childCube[i]->getChildCount();
            }
        }
        count ++;   //Count ourself
        return count;
    }
    else{
        return 0;
    }
}

int I3C_EditingCube::getPixelCount()
{
    if(m_width == 2){
        return numberHighBits(m_map);
    }
    int count = 0;
    for(int i = 0; i < 8; i++){
        if((m_map & (0x01 << i)) != 0){
            count += m_childCube[i]->getPixelCount();
        }
    }
    return count;
}

unsigned char I3C_EditingCube::cubeId(int* x, int* y, int* z)
{
    unsigned char cube = 0;
    int halfWidth = m_width/2;

    //Figure out cube
    if(*y < halfWidth){
        cube += 4;
    }
    if(*z < halfWidth){
        cube += 2;
    }
    if(*z > halfWidth && *x < halfWidth){
        cube += 1;
    }
    if(*z < halfWidth && *x > halfWidth){
        cube += 1;
    }

    //Update position for child call
    *x = RECENTER(*x, halfWidth);
    *y = RECENTER(*y, halfWidth);
    *z = RECENTER(*z, halfWidth);

    return cube;
}

void I3C_EditingCube::deleteChild(unsigned char cubeId)
{
    delete m_childCube[cubeId];
    m_childCube[cubeId] = NULL;
    m_map &= ~(0x01 << cubeId);
    return;
}

//#endif
