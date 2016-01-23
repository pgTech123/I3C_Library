#include "i3c_write.h"

I3C_Write::I3C_Write()
{
    m_fileEmpty = true;
    m_ImageFile = new ImageFile;
}

I3C_Write::~I3C_Write()
{
    this->close();
    delete m_ImageFile;
}

int I3C_Write::open(const char* filename)
{
    m_file.open(filename, ios_base::out);

    //Check if file open
    if(!m_file.is_open()){
        return I3C_ERR_FILE_NOT_OPEN;
    }

    return I3C_SUCCESS;
}

void I3C_Write::close()
{
    //Close file if it was opened
    if(!m_file.is_open()){
        m_file.close();
    }
}

int I3C_Write::writeImage(I3C_Frame* frame, int imgFormat)
{
    m_ImageFile->writeFrame(&m_file, frame, imgFormat);
    return I3C_SUCCESS;
}

int I3C_Write::writeVideo(/*TODO*/)
{
    //TODO
    return I3C_SUCCESS;
}

bool I3C_Write::isFileEmpty()
{
    return m_fileEmpty;
}
