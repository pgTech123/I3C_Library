#include "i3c_read.h"

I3C_Read::I3C_Read()
{
    m_b_isVideo = false;
    m_b_isImage  =false;
    m_pFrame_Buf1 = NULL;
    m_pFrame_Buf2 = NULL;
    m_ImageFile = new ImageFile;
}

I3C_Read::~I3C_Read()
{
    this->close();

    delete m_ImageFile;
}

int I3C_Read::open(const char* filename)
{
    m_file.open(filename);

    //Check if file open
    if(!m_file.is_open()){
        return I3C_ERR_FILE_NOT_FOUND;
    }

    //Check if Image or Video
    if(m_ImageFile->isImage(&m_file)){
        m_b_isImage = true;
        m_b_isVideo = false;
    }
    /*else if(isVideo(m_file)){
        m_b_isImage = false;
        m_b_isVideo = true;
    }*/
    else{
        return I3C_ERR_COMPRESS_NOT_FOUND;
    }

    return I3C_SUCCESS;
}

void I3C_Read::close()
{
    this->stopStream();

    //Close file if it was opened
    if(!m_file.is_open()){
        m_file.close();
    }
}

int I3C_Read::read(I3C_Frame* frame, int frameNumber)
{
    if(!m_file.is_open()){
        return I3C_ERR_FILE_NOT_FOUND;
    }
    else if(m_b_isImage){
        return m_ImageFile->readFrame(&m_file, frame);
    }
    else if(m_b_isVideo){
        //TODO
    }
    return I3C_ERR_COMPRESS_NOT_FOUND;
}

void I3C_Read::startStream(/*Callback(I3C_VideoFrame* frame)*/)
{
    //TODO
}

void I3C_Read::stopStream()
{
    //TODO
}

bool I3C_Read::isVideoFile()
{
    return m_b_isVideo;
}

bool I3C_Read::isImageFile()
{
    return m_b_isImage;
}
