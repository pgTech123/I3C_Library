//Author:           Pascal Gendron
//Creation date:    September 4th 2015
//Description:      Read Image (Back-end)
//
//Additionnal info: When a new image compression is created,
//                  the header ID should be added in this file.
//                  An case must also be added in |readFrame()|:
//                  put the function that has the ability to
//                  read your file compression there.
//                  Places to modify are tagged as <NEW FILE TYPE> (ctrl-f).

#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include <stdio.h>
#include <fstream>

#include "../../i3c.h"
#include "image_v1.h"

// Header ID
// WARNING: Image Header id must NOT interfere with Video Header id
#define I3C_IMAGE_V1    0x00000000  //Special case: No header in this file
#define I3C_IMAGE_V2    0x00000001
// <NEW FILE TYPE>: Additionnal Header ID must be added here...

using namespace std;

class ImageFile
{
public:
    ImageFile();

    // |ImageFile()| and |readFrame()| reads the
    // file and fills I3C_Frame.
    int readFrame(fstream *file, I3C_Frame* frame);     //Returns error code

    // |isImage()| returns true if the header is an image header
    bool isImage(fstream *file);

private:
    // |readHeader()| reads the version of the file
    // and returns the Version
    int readHeader(fstream *file);

};

#endif // IMAGEFILE_H
