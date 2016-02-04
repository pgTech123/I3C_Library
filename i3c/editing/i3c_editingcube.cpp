//#ifdef I3C_EDITING_MODULE_ENABLED

#include "i3c_editingcube.h"

I3C_EditingCube::I3C_EditingCube(int width)
{
    m_width = width;

    for(int i = 0; i < 8; i++){
        m_childCube[i] = NULL;
    }
}

I3C_EditingCube::~I3C_EditingCube()
{
    for(int i = 0; i < 8; i++){
        if(m_childCube[i] != NULL){
            delete m_childCube[i];
        }
    }
}

void I3C_EditingCube::addCube(int x, int y, int z, Pixel pixel)
{
    if(m_width == 1){
        m_avgPixel = pixel;
    }
    else{
        int halfWidth = m_width/2;
        unsigned char cube = 0;

        //TODO: Figure out cube

        //Update position for child call
        x = RECENTER(x, halfWidth);
        y = RECENTER(y, halfWidth);
        z = RECENTER(z, halfWidth);

        if((m_map & (0x01 << cube)) == 0){
            //Child cube did not exist so we create it
            m_childCube[cube] = new I3C_EditingCube(halfWidth);

            //Update map
            m_map &= (0x01 << cube);
        }
        //Call addCube on child
        m_childCube[cube]->addCube(x, y, z, pixel);
    }
}

void I3C_EditingCube::removeCube(int x, int y, int z)
{
    //If cube empty: delete
}

void I3C_EditingCube::getPixelAt(int x, int y, int z, Pixel* pixel)
{
    //TODO
}

//#endif
