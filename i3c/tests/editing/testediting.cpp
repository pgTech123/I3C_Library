#include "testediting.h"

TestEditing::TestEditing()
{
}

int TestEditing::testWritingReading()
{
    const char* filenameR = TEST_FILE;

    I3C_Read i3cFileR;
    I3C_Frame frame;
    I3C_Frame frameReturned;
    int error;

    //Load a test image
    error = i3cFileR.open(filenameR);
    if(error == I3C_SUCCESS){
        i3cFileR.read(&frame);
    }
    i3cFileR.close();

    I3C_Cube editingCube;

    //Write image to editing cube
    editingCube.rgb2cube(&frame);
    //Read image from editing cube (should be the same as what has been written)
    editingCube.cube2rgb(&frameReturned);

    /*
    // FURTHER DEBUG
    cout << "Resolution: " << frame.resolution << " , " << frameReturned.resolution << endl;
    cout << "Number Of levels: " << frame.numberOfLevels << " , " << frameReturned.numberOfLevels << endl;
    cout << "Map array size: " << frame.cubeMapArraySize << " , " << frameReturned.cubeMapArraySize << endl;
    cout << "Pixel array size: " << frame.pixelArraySize << " , " << frameReturned.pixelArraySize << endl;
    //*/

    //Compare frame parameters
    if(frame.resolution != frameReturned.resolution){
        logs << "Editing resolution failed: " << frame.resolution << " ! = " << frameReturned.resolution << endl;
        return I3C_EDITING_FAIL_RESOLUTION;
    }
    if(frame.numberOfLevels != frameReturned.numberOfLevels){
        logs << "Editing numOfLevel failed: " << frame.numberOfLevels << " ! = "
             << frameReturned.numberOfLevels << endl;
        return I3C_EDITING_FAIL_NUMOFLEVEL;
    }
    if(frame.cubeMapArraySize != frameReturned.cubeMapArraySize){
        logs << "Editing map array size failed: " << frame.cubeMapArraySize << " ! = "
             << frameReturned.cubeMapArraySize << endl;
        return I3C_EDITING_FAIL_MAPARRAYSIZE;
    }
    if(frame.pixelArraySize != frameReturned.pixelArraySize){
        logs << "Editing pixel array size failed: " << frame.pixelArraySize << " ! = "
             << frameReturned.pixelArraySize << endl;
        return I3C_EDITING_FAIL_PIXARRAYSIZE;
    }
    for(int i = 0; i < frame.numberOfLevels; i++){
        if(frame.mapAtLevel[i] != frameReturned.mapAtLevel[i]){
            logs << "Editing map at level failed at index: " << i << " -> " << (int)frame.mapAtLevel[i]
                    << " ! = " << (int)frameReturned.mapAtLevel[i] << endl;
            return I3C_EDITING_FAIL_MAPATLEVEL;
        }
    }
    for(unsigned int i = 0; i < frame.cubeMapArraySize; i++){
        //Check for map
        if(frame.cubeMap[i] != frameReturned.cubeMap[i]){
            logs << "Editing map array failed at index: " << i << " -> " << (int)frame.cubeMap[i]
                    << " ! = " << (int)frameReturned.cubeMap[i] << endl;
            return I3C_EDITING_FAIL_MAP;
        }
        //Check maps and child IDs
        if(frame.childCubeId[i] != frameReturned.childCubeId[i]){
            logs << "Editing childID array failed at index: " << i << " -> " << frame.childCubeId[i]
                    << " ! = " << frameReturned.childCubeId[i] << endl;
            return I3C_EDITING_FAIL_CHILDID;
        }
    }
    for(unsigned int i = 0; i < frame.pixelArraySize; i++){
        //Check pixels
        if( frame.pixel[i].red != frameReturned.pixel[i].red ||
            frame.pixel[i].green != frameReturned.pixel[i].green ||
            frame.pixel[i].blue != frameReturned.pixel[i].blue){
            logs << "Editing pixel array (R) failed at index: " << i << " -> " << (int)frame.pixel[i].red
                    << " ! = " << (int)frameReturned.pixel[i].red << endl;
            logs << "Editing pixel array (G) failed at index: " << i << " -> " << (int)frame.pixel[i].green
                    << " ! = " << (int)frameReturned.pixel[i].green << endl;
            logs << "Editing pixel array (B) failed at index: " << i << " -> " << (int)frame.pixel[i].blue
                    << " ! = " << (int)frameReturned.pixel[i].blue << endl;
            return I3C_EDITING_FAIL_PIX;
        }
    }

    logs << "Editing Cubes test: Pass" << endl;

    return error;
}

