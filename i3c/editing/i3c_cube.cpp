#include "i3c_cube.h"

//#ifdef I3C_EDITING_MODULE_ENABLED

I3C_Cube::I3C_Cube(int resolution)
{
    if(!is_Base2(resolution)){
        resolution = DEFAULT_CUBE_WIDTH;
    }
    m_editingCube = new I3C_EditingCube(resolution);
    m_width = resolution;
}

I3C_Cube::~I3C_Cube()
{
    if(m_editingCube != NULL){
        delete m_editingCube;
    }
}

int I3C_Cube::addPixel(int x, int y, int z, Pixel pixel)
{
    if(x < 0 || y < 0 || z < 0 || x > m_width || y > m_width || z > m_width){
        return I3C_CUBEEDIT_INVALID_POS;
    }
    if(m_editingCube == NULL){
        return I3C_CUBEEDIT_NOT_SET;
    }
    m_editingCube->addCube(x, y, z, pixel);
    return I3C_SUCCESS;
}

int I3C_Cube::removePixel(int x, int y, int z)
{
    if(x < 0 || y < 0 || z < 0 || x > m_width || y > m_width || z > m_width){
        return I3C_CUBEEDIT_INVALID_POS;
    }
    if(m_editingCube == NULL){
        return I3C_CUBEEDIT_NOT_SET;
    }
    m_editingCube->removeCube(x, y, z);
    return I3C_SUCCESS;
}

int I3C_Cube::getPixelAt(int x, int y, int z, Pixel* pixel)
{
    if(x < 0 || y < 0 || z < 0 || x > m_width || y > m_width || z > m_width){
        return I3C_CUBEEDIT_INVALID_POS;
    }
    if(m_editingCube == NULL){
        return I3C_CUBEEDIT_NOT_SET;
    }
    m_editingCube->getPixelAt(x, y, z, pixel);
    return I3C_SUCCESS;
}

void I3C_Cube::resetEditingCube(int resolution)
{
    if(resolution == -1){
        resolution = m_width;
    }
    else if(resolution < 0){
        resolution = DEFAULT_CUBE_WIDTH;
    }
    else if(!is_Base2(resolution)){
        resolution = DEFAULT_CUBE_WIDTH;
    }
    if(m_editingCube != NULL){
        delete m_editingCube;
    }
    m_editingCube = new I3C_EditingCube(resolution);
    m_width = resolution;
}

void I3C_Cube::cube2rgb(I3C_Frame *rgbFrame)
{
    rgbFrame->lock();
    rgbFrame->clear();

    rgbFrame->resolution = m_width;
    rgbFrame->numberOfLevels = firstHighBit(m_width);

    //Allocate arrays
    rgbFrame->mapAtLevel = new int[rgbFrame->numberOfLevels];

    rgbFrame->pixelArraySize = m_editingCube->getPixelCount();
    rgbFrame->cubeMapArraySize = m_editingCube->getChildCount();

    rgbFrame->pixel = new Pixel[rgbFrame->pixelArraySize];
    rgbFrame->cubeMap = new unsigned char[rgbFrame->cubeMapArraySize];
    rgbFrame->childCubeId = new unsigned int[rgbFrame->cubeMapArraySize];

    //Get Pixels
    m_editingCube->fillPixelArray(rgbFrame->pixel, rgbFrame->pixelArraySize-1, rgbFrame->numberOfLevels);

    //Get Maps
    int id = rgbFrame->cubeMapArraySize-1;
    int idInitial;
    for(int i = 0; i < rgbFrame->numberOfLevels; i++){
        idInitial = id;
        id = m_editingCube->fillMapArray(rgbFrame->cubeMap, id, i);

        //Update map at level
        rgbFrame->mapAtLevel[rgbFrame->numberOfLevels-i-1] = idInitial-id;
    }

    //Get child ID (Pixels)
    int index = 0;
    for(int i = 0 ; i < rgbFrame->mapAtLevel[0]; i++){
        rgbFrame->childCubeId[i] = index;
        index += numberHighBits(rgbFrame->cubeMap[i]);
    }
    //Get child ID (Maps)
    index = 0;
    for(int i = rgbFrame->mapAtLevel[0] ; i < rgbFrame->cubeMapArraySize; i++){
        rgbFrame->childCubeId[i] = index;
        index += numberHighBits(rgbFrame->cubeMap[i]);
    }

    rgbFrame->unlock();
}

void I3C_Cube::cube2yuv(/*TODO*/)
{
    //TODO
}

int I3C_Cube::rgb2cube(I3C_Frame *rgbFrame)
{
    rgbFrame->lock();
    //Remove existing editing cube
    this->resetEditingCube(rgbFrame->resolution);

    //Set new one
    m_editingCube->setCubes(rgbFrame->pixel, rgbFrame->cubeMap,
                            rgbFrame->childCubeId, rgbFrame->cubeMapArraySize-1);
    rgbFrame->unlock();
    return I3C_SUCCESS;
}

void I3C_Cube::yuv2cube(/*TODO*/)
{
    //TODO
}

int I3C_Cube::getWidth()
{
    return m_width;
}

//#endif
