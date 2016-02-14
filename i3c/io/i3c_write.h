//Author:           Pascal Gendron
//Creation date:    August 31th 2015
//Description:      Write Functions Front-End

#ifndef I3C_WRITE_H
#define I3C_WRITE_H

#include <stdio.h>
#include <fstream>

#include "i3c.h"
#include "Images/imagefile.h"
#include "Video/videofile.h"

class I3C_Write
{
public:
    I3C_Write();
    ~I3C_Write();

    int open(const char* filename);
    void close();

    int writeImage(I3C_Frame* frame, int imgFormat);
    int writeVideo(/*TODO*/);

    // Return wether the file already existed, if an image or if a video has been written
    bool isFileEmpty();

private:
    fstream m_file;
    ImageFile* m_ImageFile;

    bool m_fileEmpty;
};

#endif // I3C_WRITE_H
