#include "testediting.h"

TestEditing::TestEditing()
{
}

bool TestEditing::testWritingReading()
{
    const char* filenameR = "camTest.i3c";

    I3C_Read i3cFileR;
    I3C_Frame frame;
    I3C_Frame frameReturned;
    int error;

    error = i3cFileR.open(filenameR);
    if(error == I3C_SUCCESS){
        i3cFileR.read(&frame);
    }
    i3cFileR.close();

    I3C_Cube editingCube;

    editingCube.rgb2cube(&frame);
    editingCube.cube2rgb(&frameReturned);

    cout << "Resolution: " << frame.resolution << " , " << frameReturned.resolution << endl;
    cout << "Number Of levels: " << frame.numberOfLevels << " , " << frameReturned.numberOfLevels << endl;
    cout << "Map array size: " << frame.cubeMapArraySize << " , " << frameReturned.cubeMapArraySize << endl;
    cout << "Pixel array size: " << frame.pixelArraySize << " , " << frameReturned.pixelArraySize << endl;

    return error;
}

