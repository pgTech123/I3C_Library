#include "gl_i3c_element.h"

GL_I3C_Element::GL_I3C_Element()
{
    this->initElement();
}

GL_I3C_Element::GL_I3C_Element(const char* filename)
{
    this->initElement();
    this->loadFile(filename);
}

GL_I3C_Element::~GL_I3C_Element()
{
    m_i3cFile.close();

    this->delete_m_Frame();
    this->deleteCLMemObj();
}

bool GL_I3C_Element::loadFile(const char* filename)
{
    //Make sure there is no file already open
    m_i3cFile.close();
    m_fileLoaded = false;

    //Open the file
    if(m_i3cFile.open(filename)==I3C_SUCCESS){
        m_fileLoaded = true;

        //Make sure to not override an existing pointer
        this->delete_m_Frame();
        m_frame = new I3C_Frame();
    }

    //If an OpenCL context is available, load the image on GPU
    if(m_OCLContextIsSet && m_fileLoaded){
        this->prepareOCL();
        this->loadFrameOCL();
    }

    return m_fileLoaded;
}

void GL_I3C_Element::play()
{
    if(!m_fileLoaded || !m_i3cFile.isVideoFile()){
        return;
    }
    m_videoPlaying = true;
    //TODO: m_i3cFile.startStream(/*Callback TODO*/);   //start streaming
}

void GL_I3C_Element::pause()
{
    if(!m_fileLoaded || !m_i3cFile.isVideoFile()){
        return;
    }
    m_videoPlaying = false;
    //TODO: m_i3cFile.stopStream();         //stop streaming
}

void GL_I3C_Element::setTransform(float x, float y, float z,
                                 float pitch, float yaw, float roll,
                                 float sx, float sy, float sz)
{
    Position pos;
    Axis rotation;
    Axis scale;

    pos.x = x;
    pos.y = y;
    pos.z = z;

    rotation.x = pitch;
    rotation.y = yaw;
    rotation.z = roll;

    scale.x = sx;
    scale.y = sy;
    scale.z = sz;

    matFromPosAndRot(&m_originalCoord, pos, rotation, scale);
}

void GL_I3C_Element::setTransform(Mat4x4 transform)
{
    //Copy matrix
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            m_originalCoord.element[i][j] = transform.element[i][j];
        }
    }
}

void GL_I3C_Element::setViewTransformMatrix(Mat4x4* transformMatrix)
{
    m_transformMatrix = transformMatrix;
}

bool GL_I3C_Element::setOCLContext(cl_context* context, cl_program* program, cl_command_queue* queue)
{
    //Make sure to not overide an existing context
    if(m_OCLContextIsSet){
        return false;
    }

    //Set Context
    m_clContext = context;
    m_clQueue = queue;
    m_OCLContextIsSet = true;

    //Creation of the kernels
    m_clRenderingKernel = clCreateKernel(*program, "render", NULL);
    m_clClearTextureKernel = clCreateKernel(*program, "clearTexture", NULL);
    m_clClearKernel = clCreateKernel(*program, "clearMemoryBit", NULL);
    m_clLoadVideoBufferKernel = clCreateKernel(*program, "loadVideoBuffer", NULL);
    //TODO: Create other kernels here

    //If a file was already specified, load it
    if(m_fileLoaded){
        this->prepareOCL();
        this->loadFrameOCL();
    }
    return true;
}

void GL_I3C_Element::resetOCLContext()
{
    this->pause();          //Stop streaming
    this->deleteCLMemObj();
    this->initElement();
}

void GL_I3C_Element::setScreenSize(int w, int h)
{
    m_screenWidth = w;
    m_screenHeight = h;
}

void GL_I3C_Element::setTextures(cl_mem* renderingTexture, cl_mem* depthMap)
{
    m_clRenderingTexture = renderingTexture;
    m_clDepthMap = depthMap;

    //TODO: set Arguments
    clSetKernelArg(m_clRenderingKernel, 0, sizeof(*m_clRenderingTexture), m_clRenderingTexture);
    clSetKernelArg(m_clClearTextureKernel, 0, sizeof(*m_clRenderingTexture), m_clRenderingTexture);

    m_texturesSpecified = true;
}

void GL_I3C_Element::render()
{
    if(m_texturesSpecified){
        //TODO: Start clearing mem bit
        cl_int error;

        //Compute transform
        computeTransform(&m_originalCoord, &m_transformedObject, m_transformMatrix);
        projectObject(&m_transformedObject, &m_ObjectBoundOnScreen, m_screenWidth, m_screenHeight);

        glFinish(); //FIXME: IMPROVE THAT...
        error = clEnqueueAcquireGLObjects(*m_clQueue, 1, m_clRenderingTexture, 0, 0, NULL);
        //cout << "Texture acquired!  " << error << endl;

        //DEBUG
        size_t wi_clear[2] = {m_screenWidth, m_screenHeight};
        error = clEnqueueNDRangeKernel(*m_clQueue, m_clClearTextureKernel, 2, NULL, wi_clear , NULL, 0, NULL, NULL);


        //Update Object Boundaries
        cl_int2 objectScreenOffset;
        objectScreenOffset.s[0] = (cl_int)m_ObjectBoundOnScreen.x;
        objectScreenOffset.s[1] = (cl_int)m_ObjectBoundOnScreen.y;
        clEnqueueWriteBuffer(*m_clQueue, m_clObjectOffset, CL_TRUE, 0, sizeof(cl_int2),
                             &objectScreenOffset, 0, NULL, NULL);

        //Create the appropriate amount of workitems (w and h from m_ObjectBoundOnScreen)
        size_t wi[2] = {m_ObjectBoundOnScreen.w, m_ObjectBoundOnScreen.h};
        error = clEnqueueNDRangeKernel(*m_clQueue, m_clRenderingKernel, 2, NULL, wi , NULL, 0, NULL, NULL);
        if(error == CL_INVALID_KERNEL_ARGS){
            cout << "Invalid Arg" << endl;
        }
        else if(error == CL_SUCCESS){
            // cout << "OCL WORK!  " << endl;
        }

        clFinish(*m_clQueue);
        error = clEnqueueReleaseGLObjects(*m_clQueue, 1, m_clRenderingTexture, 0, 0, NULL);
        //cout << "Texture released!  " << error << endl;
    }
}

void GL_I3C_Element::initElement()
{
    m_OCLContextIsSet = false;
    m_texturesSpecified = false;

    m_fileLoaded = false;
    m_videoPlaying = false;

    m_frame = NULL;

    this->initCLMemObj();
    this->initPosition();
    this->initTransformMat();
}

void GL_I3C_Element::initPosition()
{
    this->setTransform();   //Default
}

void GL_I3C_Element::initTransformMat()
{
    m_transformMatrix = NULL;
}

void GL_I3C_Element::prepareOCL()
{
    if(m_frame == NULL){
        return;
    }

    //Get the space needed
    m_i3cFile.read(m_frame);

    //Clear mem previously allocated
    this->releaseArguments();

    //Allocate memory
    m_clPixels = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                m_frame->pixelArraySize*sizeof(cl_char3), NULL, NULL);
    m_clCubesMap = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                  m_frame->cubeMapArraySize*sizeof(cl_char), NULL, NULL);
    m_clChildId = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                 m_frame->cubeMapArraySize*sizeof(cl_int), NULL, NULL);
    m_clObjectOffset = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                      sizeof(cl_int2), NULL, NULL);

    //Set Args
    //TODO
    clSetKernelArg(m_clRenderingKernel, 1, sizeof(m_clObjectOffset), &m_clObjectOffset);

    //Allocate video memory
    if(m_i3cFile.isVideoFile()){
        //TODO: allocate video mem
    }
}

void GL_I3C_Element::loadFrameOCL()
{
    //TODO: get frame and transfer on OCL mem (if video, load first frame, maybe fill buffers)
}

void GL_I3C_Element::initCLMemObj()
{
    //Context & Queue ptr
    m_clContext = NULL;
    m_clQueue = NULL;

    //Texture Mem ptr
    this->forgetTextures();

    //Kernels
    m_clRenderingKernel = NULL;
    m_clClearKernel = NULL;
    m_clLoadVideoBufferKernel = NULL;
    m_clClearTextureKernel = NULL;

    //Mem (Arguments)
    m_clPixels = NULL;
    m_clCubesMap = NULL;
    m_clChildId = NULL;
    m_clDiffPixels = NULL;
    m_clDiffCubesMap = NULL;
    m_clDiffChildId = NULL;
    m_clObjectOffset = NULL;
}

void GL_I3C_Element::releaseKernels()
{
    //Release Kernels
    if(m_clRenderingKernel != NULL){
        clReleaseKernel(m_clRenderingKernel);
        m_clRenderingKernel = NULL;
    }
    if(m_clClearKernel != NULL){
        clReleaseKernel(m_clClearKernel);
        m_clClearKernel = NULL;
    }
    if(m_clLoadVideoBufferKernel != NULL){
        clReleaseKernel(m_clLoadVideoBufferKernel);
        m_clLoadVideoBufferKernel = NULL;
    }
    if(m_clClearTextureKernel != NULL){
        clReleaseKernel(m_clClearTextureKernel);
        m_clClearTextureKernel = NULL;
    }
}

void GL_I3C_Element::releaseArguments()
{
    //Release Argiments
    if(m_clPixels != NULL){
        clReleaseMemObject(m_clPixels);
        m_clPixels = NULL;
    }
    if(m_clCubesMap != NULL){
        clReleaseMemObject(m_clCubesMap);
        m_clCubesMap = NULL;
    }
    if(m_clChildId != NULL){
        clReleaseMemObject(m_clChildId);
        m_clChildId = NULL;
    }
    if(m_clDiffPixels != NULL){
        clReleaseMemObject(m_clDiffPixels);
        m_clDiffPixels = NULL;
    }
    if(m_clDiffCubesMap != NULL){
        clReleaseMemObject(m_clDiffCubesMap);
        m_clDiffCubesMap = NULL;
    }
    if(m_clDiffChildId != NULL){
        clReleaseMemObject(m_clDiffChildId);
        m_clDiffChildId = NULL;
    }
    if(m_clObjectOffset != NULL){
        clReleaseMemObject(m_clObjectOffset);
        m_clObjectOffset = NULL;
    }
}

void GL_I3C_Element::forgetTextures()
{
    //Point to NULL what's no managed here
    m_clRenderingTexture = NULL;    //!!! NOT MANAGED IN THIS CLASS: DO NOT RELEASE !!!
    m_clDepthMap = NULL;            //!!! NOT MANAGED IN THIS CLASS: DO NOT RELEASE !!!
    m_texturesSpecified = false;
}

void GL_I3C_Element::deleteCLMemObj()
{
    this->releaseKernels();
    this->releaseArguments();
    this->forgetTextures();
}

void GL_I3C_Element::delete_m_Frame()
{
    if(m_frame != NULL){
        delete m_frame;
        m_frame = NULL;
    }
}
