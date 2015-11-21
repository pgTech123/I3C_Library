//Author:           Pascal Gendron
//Creation date:    September 6th 2015
//Description:      Used to represent an I3C element
//                  that will eventualy be rendered. This
//                  class gives it a position, a size, ...
//
//                  * A GL_I3C_Element can only be used in
//                    one scene at the time (1 context and
//                    one transform matrix(ptr)).

#ifndef GL_I3C_ELEMENT_H
#define GL_I3C_ELEMENT_H

#include <GL/glut.h>
#include "CL/cl.h"
#include <CL/cl_gl.h>

#include "../io/i3c_read.h"
#include "i3c_transform.h"


#define VIDEO_BUFFER_SIZE   4



class GL_I3C_Element
{
public:
    GL_I3C_Element();
    GL_I3C_Element(const char* filename);
    virtual ~GL_I3C_Element();

    bool loadFile(const char* filename);

    void play();
    void pause();

    void setTransform(float x=0, float y=0, float z=0,
                      float pitch=0, float yaw=0, float roll=0,
                      float sx=1, float sy=1, float sz=1);
    void setTransform(Mat4x4 transform);

    //!!!------------------------------------------------------------
    //!  These functions are NOT meant to be called by the end user
    //! -------------------------------------------------------------
    bool setOCLContext(cl_context* context, cl_program* program, cl_command_queue* queue);
    void resetOCLContext();

    void setScreenSize(int w, int h);

    void setViewTransformMatrix(Mat4x4* transformMatrix);

    void setTextures( cl_mem* renderingTexture, cl_mem* depthMap);

    void render();
    //!!!------------------------------------------------------------
    //!                             END
    //! -------------------------------------------------------------

private:
    void initElement();
    void initPosition();
    void initTransformMat();

    void prepareOCL();
    void loadFrameOCL();

    void initCLMemObj();

    void releaseKernels();
    void releaseArguments();
    void forgetTextures();

    void deleteCLMemObj();
    void delete_m_Frame();

    void acquireGLTexture();
    void releaseGLTexture();

    void enqueueSetScreenBoundaries();
    void enqueueSetCubeCorners();

    void enqueueClearTexture();
    void enqueueRender();

private:
    //File
    I3C_Read m_i3cFile;
    bool m_fileLoaded;

    //Frame
    I3C_Frame *m_frame;

    //Video Variables
    bool m_videoPlaying;

    //Coordinates in space
    Mat4x4 m_originalCoord;
    ObjectBoundaries m_transformedObject;
    Mat4x4 *m_transformMatrix;
    ScreenBoundaries m_ObjectBoundOnScreen;

    //Screen Size
    int m_screenWidth;
    int m_screenHeight;

    //OCL context status
    bool m_OCLContextIsSet;
    bool m_texturesSpecified;

    //OCL Context
    cl_context* m_clContext;
    cl_command_queue* m_clQueue;

    //Kernels
    cl_kernel m_clRenderingKernel;
    cl_kernel m_clClearTextureKernel;       //DEBUG ONLY
    cl_kernel m_clClearKernel;
    cl_kernel m_clLoadVideoBufferKernel;

    //OpenCL image cube memory
    cl_mem m_clCubeCorners;
    cl_mem m_clObjectOffset;   //As the whole screen won't be rendered, specify where to start

    //OpenCL image/video memory
    cl_mem* m_clRenderingTexture;    //!!! THIS IS MANAGED OUTSIDE THIS CLASS: do not release
    cl_mem* m_clDepthMap;            //!!! THIS IS MANAGED OUTSIDE THIS CLASS: do not release

    cl_mem m_clPixels;
    cl_mem m_clCubesMap;
    cl_mem m_clChildId;

    //OpenCL video memory
    cl_mem m_clDiffPixels;
    cl_mem m_clDiffCubesMap;
    cl_mem m_clDiffChildId;
};

#endif // GL_I3C_ELEMENT_H
