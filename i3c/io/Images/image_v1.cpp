#include "image_v1.h"

ImageV1::ImageV1()
{
    m_pi_mapsAtLevel = NULL;
}

ImageV1::~ImageV1()
{
    this->clearMapsAtLevel();
}

int ImageV1::read(fstream* file, I3C_Frame* frame)
{
    if(frame != NULL){
        frame->clear();
    }

    //Make sure we're at the begining
    file->clear();
    file->seekg (0, ios::beg);

    //Read Side Size
    int error = this->readSideSize(file);
    frame->resolution = m_i_sideSize;
    if(error != I3C_SUCCESS){
        return error;
    }

    //Levels
    m_i_totalMaps = 0;
    m_i_numberOfLevels = firstHighBit(m_i_sideSize);
    m_pi_mapsAtLevel = new int[m_i_numberOfLevels];
    for(int i = 0; i < m_i_numberOfLevels; i++)
    {
        *file >> m_i_buffer;
        m_pi_mapsAtLevel[i] = m_i_buffer;
        m_i_totalMaps += m_i_buffer;
    }
    frame->cubeMapArraySize = m_i_totalMaps;

    //Allocate memory for map & childId
    frame->childCubeId = new unsigned int[frame->cubeMapArraySize];
    frame->cubeMap = new unsigned char[frame->cubeMapArraySize];

    //Read pixels values
    error = readPixels(file, frame);
    if(error != I3C_SUCCESS){
        this->clearMapsAtLevel();
        return error;
    }

    //Read maps
    error = readParents(file, frame);
    if(error != I3C_SUCCESS){
        this->clearMapsAtLevel();
        return error;
    }

    this->clearMapsAtLevel();
    return I3C_SUCCESS;
}

int ImageV1::readSideSize(fstream* file)
{
    *file >> m_i_sideSize;

    if(m_i_sideSize <= 1 || !is_Base2(m_i_sideSize)){
        m_i_sideSize = 0;
        return I3C_INVALID_IMAGE_SIZE;
    }

    return I3C_SUCCESS;
}

int ImageV1::readPixels(fstream* file, I3C_Frame* frame)
{
    int error, pixelsInCube, totalPixels = 0, pixelOffset = 0;
    unsigned char map, red, green, blue;

    //Keep a marker in the file
    int marker = file->tellg();

    //Count pixels
    for(int i = 0; i < m_pi_mapsAtLevel[0]; i++){
        error = readMap(file, &map, &pixelsInCube);
        if(error != I3C_SUCCESS){
            return error;
        }
        for(int j = 0; j < pixelsInCube; j++)
        {
            //Pixel Reading
            *file >> red;
            *file >> green;
            *file >> blue;
        }
        totalPixels += pixelsInCube;
    }

    //Allocate pixels
    frame->pixelArraySize = totalPixels;
    frame->pixel = new Pixel[totalPixels];

    //Read a second time for data
    //Depending of the OS, line ends with \r\n or \n. To be able to
    //read aby of these files, we rewind of 16 bits and we read til we
    //find a line that contains only '1'.
    file->seekg(marker-16);
    string line;
    int iter = 0;   //Safety
    getline(*file, line);
    while(line != "1" && iter < 5){
        getline(*file,line);
        iter++;
    }

    //Read Pixels
    for(int i = 0; i < m_pi_mapsAtLevel[0]; i++)
    {
        error = readMap(file, &map, &pixelsInCube);
        if(error != I3C_SUCCESS){
            return error;
        }
        for(int j = 0; j < pixelsInCube; j++)
        {
            //Pixel Reading
            *file >> red;
            *file >> green;
            *file >> blue;

            frame->pixel[pixelOffset+j].red = (unsigned char)red;
            frame->pixel[pixelOffset+j].green = (unsigned char)green;
            frame->pixel[pixelOffset+j].blue = (unsigned char)blue;
        }
        frame->cubeMap[i] = map;
        frame->childCubeId[i] = pixelOffset;

        pixelOffset += pixelsInCube;
    }

    //Read pixels
    return I3C_SUCCESS;
}

int ImageV1::readParents(fstream* file, I3C_Frame* frame)
{
    unsigned char map;
    int error, numOfChild=0, offset=0;

    //Read the maps of each upper level
    for(int i = m_pi_mapsAtLevel[0]; i < m_i_totalMaps; i++){
        error = readMap(file, &map, &numOfChild);
        if(error != I3C_SUCCESS){
            return error;
        }
        frame->cubeMap[i] = map;
        frame->childCubeId[i] = offset;
        offset += numOfChild;
    }

    return I3C_SUCCESS;
}

int ImageV1::readMap(fstream* file, unsigned char* ucMap, int* iNumOfPix)
{
    int iBufMap;
    *file >> iBufMap;
    *iNumOfPix = numberHighBits(iBufMap);
    if(*iNumOfPix > 8){
        return I3C_FILE_CORRUPTED;
    }
    *ucMap = (unsigned char)iBufMap;

    return I3C_SUCCESS;
}

void ImageV1::clearMapsAtLevel()
{
    if(m_pi_mapsAtLevel!= NULL){
        delete[] m_pi_mapsAtLevel;
        m_pi_mapsAtLevel = NULL;
    }
}
