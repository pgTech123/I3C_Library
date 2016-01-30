//Author:           Pascal Gendron
//Creation date:    September 11th 2015
//Description:      Used to init the OpenCL context and to render
//                  I3C elements.

#ifndef GL_I3C_SCENE_H
#define GL_I3C_SCENE_H

#ifdef I3C_RENDER_MODULE_ENABLED

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <windows.h>

//OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
//OpenCL
#include <CL/cl.h>
#include <CL/cl_gl.h>


#include "gl_i3c_element.h"
#include "./../i3c.h"
#include "i3c_transform.h"
#include "./../utils/logs.h"



using namespace std;

#define MAX_NUMBER_OF_PLATFORM      10      // If you get the error: I3C_CANNOT_FIND_CONTEXT, when you call
                                            // |createOCLContext()|try to put a bigger number here.
#define MAX_FILE_SIZE               100000  // Default 100 Ko


//-------------    -------------    -------------    -------------    -------------
// IF MORE OPEN CL SOURCES FILES ARE NEEDED, ADJUST |NUM_OF_FILES_TO_LOAD|,
// and add the filename in |FILES_TO_LOAD|.

//Internaly, all the sources files will be appended one after the other in the order
//they appear. They'll then be compiled.

#define NUM_OF_FILES_TO_LOAD                      3

static const char* const FILES_TO_LOAD[NUM_OF_FILES_TO_LOAD] = {
                                                                "cl_sources/rendering.cl",
                                                                "cl_sources/clear.cl",
                                                                "cl_sources/video.cl"
                                                               };
//-------------    -------------    -------------    -------------    -------------

//OpenGL & OpenCL interaction
typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetGLContextInfoKHR_fn)(const cl_context_properties * /* properties */,
                                                                    cl_gl_context_info /* param_name */,
                                                                    size_t /* param_value_size */,
                                                                    void * /* param_value */,
                                                                    size_t * /*param_value_size_ret*/);


class GL_I3C_Scene
{
public:
    GL_I3C_Scene();
    ~GL_I3C_Scene();

    //This function inits the OpenCL context, on the same device as the OpenGL context.
    //This function MUST be called before anything else in this class
    int attachGLContext(HDC hDC, HGLRC hRC);

    //Returns true if the OCL context was created, and returns false if an error occured.
    bool isUpAndRunning();

    //This function must be called before |renderI3C()| can be called.
    //To update rendering textures, stopRender must be called before the texture are deleted
    bool setRenderingTextures(GLuint renderingTexture, GLuint depthMap);
    void stopRender();

    //It's the client responsability to manage the memory, i.e. create and delete |GL_I3C_Element|.
    //These functions are only used to tell the scene which GL_I3CElements it should care about.
    //Elements should be deleted only when they are retreived of the scene. Otherwise, unknown behaviour.
    bool addI3CElement(GL_I3C_Element* element);
    void retreiveI3CElement(GL_I3C_Element* element);
    void retreiveAll();

    //LookAt functions
    void lookAt(Position position, Axis rotation);
    void lookAt(float x, float y, float z, float pitch, float yaw, float roll);
    void setTransformMatrix(Mat4x4 transformMat);

    //This function is used to render I3C elements only
    void renderI3C();

private:
    void initOCL();
    void shutdownOCL();
    void releaseTextures();

    void initTransformMat();

    int createOCLContext(HDC hDC, HGLRC hRC);
    void loadOCLProgram();
    void loadOCLSources();

private:
    char* m_clSources;

    int m_ScreenWidth;
    int m_ScreenHeight;

    //OpenCL
    cl_device_id m_device;
    cl_context m_context;
    cl_command_queue m_queue;
    cl_program m_program;

    cl_mem m_clRenderingTexture;    //FIXME: Eventually change for a dynamic array
    cl_mem m_clDepthMap;            //FIXME: Eventually change for a dynamic array

    //I3C Element
    vector<GL_I3C_Element*> m_i3cElements;

    //Status
    bool m_OCL_OK;

    //LookAt
    Mat4x4 m_transformMatrix;
};

#endif

#endif // GL_I3C_SCENE_H
