#include "image_v2.h"

ImageV2::ImageV2()
{
}

ImageV2::~ImageV2()
{
}

int ImageV2::read(fstream* file, I3C_Frame* frame)
{
    cout << "Read function called V2" << endl;

    this->readResolution(file, frame);
    this->readMapAtLevel(file, frame);
    unsigned int numOfPixel = this->readNumOfPixel(file, frame);

    //TODO

    return I3C_SUCCESS;
}

int ImageV2::write(fstream* file, I3C_Frame* frame)
{
    cout << "Write function called V2" << endl;
    this->writeHeader(file);
    this->writeResolution(file, frame);
    this->writeMapAtLevel(file, frame);

    return I3C_SUCCESS;
}

int ImageV2::readResolution(fstream *file, I3C_Frame *frame)
{
    int resolution = 0;
    file->read((char*)&resolution, 4);
    //cout << resolution << endl;

    if(resolution <= 1 || !is_Base2(resolution)){
        frame->resolution = 0;
        return I3C_INVALID_IMAGE_SIZE;
    }

    frame->resolution = resolution;
    return I3C_SUCCESS;
}

int ImageV2::readMapAtLevel(fstream* file, I3C_Frame* frame)
{
    int totalMaps = 0;
    frame->numberOfLevels = firstHighBit(frame->resolution);
    frame->mapAtLevel = new int[frame->numberOfLevels];

    for(int i = 0; i < frame->numberOfLevels; i++){
        int mapsAtLevel;
        file->read((char*)&mapsAtLevel, 4);
        frame->mapAtLevel[i] = mapsAtLevel;
        totalMaps += mapsAtLevel;
        //cout << mapsAtLevel << endl;
    }
    frame->cubeMapArraySize = totalMaps;

    //Allocate memory for map & childId
    frame->childCubeId = new unsigned int[frame->cubeMapArraySize];
    frame->cubeMap = new unsigned char[frame->cubeMapArraySize];

    return I3C_SUCCESS;
}

unsigned int ImageV2::readNumOfPixel(fstream *file, I3C_Frame* frame)
{
    unsigned int numOfPixel;
    file->read((char*)&numOfPixel, 4);
    frame->pixelArraySize = numOfPixel;

    //Allocate memory for map & childId
    frame->pixel = new Pixel[numOfPixel];

    //cout << numOfPixel << endl;
    return numOfPixel;
}

void ImageV2::writeHeader(fstream* file)
{
    *file << I3C_IMAGE_V2_HEADER;
}

void ImageV2::writeResolution(fstream* file, I3C_Frame* frame)
{
    file->write(reinterpret_cast<const char *>(&frame->resolution), 4);
}

void ImageV2::writeMapAtLevel(fstream* file, I3C_Frame* frame)
{
    for(int i = 0; i < frame->numberOfLevels; i++){
        file->write(reinterpret_cast<const char *>(&frame->mapAtLevel[i]), 4);
    }
    unsigned int numberOfPixels = 0;
    for(int i = 0; i < frame->mapAtLevel[0]; i++){
        numberOfPixels += numberHighBits(frame->cubeMap[i]);
    }
    file->write(reinterpret_cast<const char *>(&numberOfPixels), 4);
}
