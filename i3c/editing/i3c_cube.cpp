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

    //TODO: fill arrays
}

void I3C_Cube::cube2yuv(/*TODO*/)
{
    //TODO
}

int I3C_Cube::rgb2cube(I3C_Frame *rgbFrame)
{
    //Remove existing editing cube
    this->resetEditingCube(rgbFrame->resolution);

    //Set new one
    m_editingCube->setCubes(rgbFrame->pixel, rgbFrame->cubeMap,
                            rgbFrame->childCubeId, rgbFrame->cubeMapArraySize-1);

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
