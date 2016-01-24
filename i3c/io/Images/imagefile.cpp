#include "imagefile.h"

ImageFile::ImageFile()
{
}

int ImageFile::readFrame(fstream *file, I3C_Frame* frame)
{
    int error;
    int fileID = this->readHeader(file);

    //Formats
    ImageV1 imgV1;
    ImageV2 imgV2;
    //<NEW FILE TYPE>: Instanciate your class here

    //Lock while frame is being read
    frame->lock();
    switch(fileID){
        case(I3C_IMAGE_V1):
            error = imgV1.read(file, frame);
            break;
        case(I3C_IMAGE_V2):
            error = imgV2.read(file, frame);
        // <NEW FILE TYPE>: Add a case(|your new header|):
        //      error = your_new_function(frame);
        default:
            error = I3C_ERR_COMPRESS_NOT_FOUND;
    }
    frame->unlock();

    return error;
}

int ImageFile::writeFrame(fstream *file, I3C_Frame* frame, int imgFormat)
{
    int error;

    //Formats
    ImageV1 imgV1;
    ImageV2 imgV2;
    //<NEW FILE TYPE>: Instanciate your class here

    frame->lock();
    switch(imgFormat){
        case(I3C_IMAGE_V1):
            error = imgV1.write(file, frame);
            break;
        case(I3C_IMAGE_V2):
            error = imgV2.write(file, frame);
        // <NEW FILE TYPE>: Add a case(|your new header|):
        //      error = your_new_function(frame);
        default:
            error = I3C_ERR_COMPRESS_NOT_FOUND;
    }
    frame->unlock();

    return error;
}

bool ImageFile::isImage(fstream *file)
{
    int header = this->readHeader(file);
    if(header == I3C_IMAGE_V1 || header == I3C_IMAGE_V2
       /*<NEW FILE TYPE>: Add " || header == <YOUR_HEADER>"*/){
        return true;
    }
    return false;
}

int ImageFile::readHeader(fstream *file)
{
    //Make sure we are at the begining of the file
    file->clear();
    file->seekg (0, ios::beg);

    //Read first character
    char c;
    file->read(&c, 1);
    //If number ASCII: Version 1
    if(c == '1' || c == '2' || c == '3' ||
       c == '4' || c == '5' || c == '6' ||
       c == '7' || c == '8' || c == '9'){
        return I3C_IMAGE_V1;
    }

    //If not V1, get version
    //Make sure we are at the begining of the file
    file->clear();
    file->seekg (0, ios::beg);

    char header[9];
    int version = I3C_IMAGE_UNDEFINED_VERSION;
    file->read(header, 8);
    header[8] = '\0';

    if(strncmp(header, I3C_IMAGE_V2_HEADER, 8) == 0){
        version = I3C_IMAGE_V2;
    }
    // <NEW FILE TYPE>: Add a if(strncmp(header, [YOUR HEADER], 8)):

    return version;
}
