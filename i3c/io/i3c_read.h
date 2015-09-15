//Author:           Pascal Gendron
//Creation date:    August 31th 2015
//Description:      Read Functions Front-End

#ifndef I3C_READ_H
#define I3C_READ_H

#include <stdio.h>
#include <fstream>

#include "i3c.h"
#include "Images/imagefile.h"
#include "Video/videofile.h"


//For video, there might be empty unused spaces in the arrays.
//Video header: largest pixel array, largest map array, largest childId array: (easier in openCL alloc)
//Read file in a thread



using namespace std;

class I3C_Read
{
public:
    I3C_Read();
    ~I3C_Read();

    // -- File actions --
    int open(const char* filename);
    bool close();

    // -- Reading operation --
    // |read()| fills the content of the pointers so that they describe the frame
    // specified. If the file is an image, the |frame| parameter is not used.
    // *** IT IS THE RESPOSABILITY OF THE USER TO DELETE |*frame| ***
    int read(I3C_Frame* frame, int frameNumber = 0);

    // |startStream()| Start calling the callback function for every frames
    void startStream(/*Callback(I3C_VideoFrame* frame)*/);
    // |stopStream()| Stop calling the callback streaming function
    void stopStream();

    // -- File information --
    // |isVideo()| returns true if the file is a video file and returns
    // false if the file is an image
    bool isVideoFile();
    // |isImage()| returns true if the file is an image file and returns
    // false if the file is a video file
    bool isImageFile();

private:
    bool m_b_isImage;
    bool m_b_isVideo;

    fstream m_file;
    ImageFile* m_ImageFile;
    VideoFile* m_VideoFile;

    I3C_Frame* m_pFrame_Buf1;    //This first buffer is used for buffer swap if video
    I3C_Frame* m_pFrame_Buf2;    //This second buffer is used for buffer swap if video
};

#endif // I3C_READ_H
