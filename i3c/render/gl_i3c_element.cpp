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
    int error = m_i3cFile.open(filename);
    if(error == I3C_SUCCESS){
        m_fileLoaded = true;

        //Make sure to not override an existing pointer
        this->delete_m_Frame();
        m_frame = new I3C_Frame();

        //Get the space needed
        m_i3cFile.read(m_frame);
    }
    else{
        logs << "i3c_error : " << error <<": GL_I3C_Element --- loadFile(): could not open..." << endl;
    }

    //If an OpenCL context is available, load the image on GPU
    if(m_OCLContextIsSet && m_fileLoaded){
        this->prepareOCL();
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
    cl_int error;
    //Make sure to not overide an existing context
    if(m_OCLContextIsSet){
        return false;
    }

    //Set Context
    m_clContext = context;
    m_clQueue = queue;
    m_OCLContextIsSet = true;

    //Creation of the kernels
    m_clRenderingKernel = clCreateKernel(*program, "render", &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setOCLContext(): could not CreateKernel Render..." << endl;
    }
    m_clClearTextureKernel = clCreateKernel(*program, "clearTexture", &error);    //DEBUG ONLY
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setOCLContext(): could not CreateKernel clearTexture..." << endl;
    }
    m_clClearKernel = clCreateKernel(*program, "clearMemoryBit", &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setOCLContext(): could not CreateKernel clearMemBit..." << endl;
    }
    m_clLoadVideoBufferKernel = clCreateKernel(*program, "loadVideoBuffer", &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setOCLContext(): could not CreateKernel loadVideoBuffer..." << endl;
    }
    //TODO: Create other kernels here

    //If a file was already specified, load it
    if(m_fileLoaded){
        this->prepareOCL();
    }
    return true;
}

void GL_I3C_Element::resetOCLContext()
{
    m_OCLContextIsSet = false;
    this->pause();          //Stop streaming
    this->deleteCLMemObj();

    //Re-init
    this->initCLMemObj();
    this->initTransformMat();
}

void GL_I3C_Element::setScreenSize(int w, int h)
{
    m_screenWidth = w;
    m_screenHeight = h;
}

void GL_I3C_Element::setTextures(cl_mem* renderingTexture, cl_mem* depthMap)
{
    cl_int error;
    m_clRenderingTexture = renderingTexture;
    m_clDepthMap = depthMap;

    //Set Texture Arguments
    error = clSetKernelArg(m_clRenderingKernel, 0, sizeof(*m_clRenderingTexture), m_clRenderingTexture);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setTextures(): could not SetKernelArg texture for kernel rendering..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 8, sizeof(*m_clDepthMap), m_clDepthMap);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setTextures(): could not SetKernelArg texture for kernel rendering..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 9, sizeof(*m_clDepthMap), m_clDepthMap);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setTextures(): could not SetKernelArg texture for kernel rendering..." << endl;
    }

    //TODO: To remove
    error = clSetKernelArg(m_clClearTextureKernel, 0, sizeof(*m_clRenderingTexture), m_clRenderingTexture); //DEBUG ONLY
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- setTextures(): could not SetKernelArg texture for kernel clearTexture..." << endl;
    }

    m_texturesSpecified = true;
}

void GL_I3C_Element::render()
{
    if(m_texturesSpecified && m_OCLContextIsSet && m_fileLoaded){
        this->acquireGLTexture();
        //Compute object transform
        //FIXME: move so that it's computed after acquire and clear are enqueued
        computeTransform(&m_originalCoord, &m_transformedObject, m_transformMatrix);
        projectObject(&m_transformedObject, &m_ObjectBoundOnScreen, m_screenWidth, m_screenHeight);

        //IF position exactly = to last position, do not clear precomputed cube positions

        //Clear previous texture
        this->enqueueClearTexture();
        this->enqueueClearMemoryBit();

        //Actual rendering
        this->enqueueSetScreenBoundaries();
        this->enqueueSetCubeCorners();
        this->enqueueRender();

        this->releaseGLTexture();
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
    cl_int error;
    if(m_frame == NULL){
        return;
    }

    //Clear mem previously allocated
    this->releaseArguments();

    //Allocate memory
    //Sorry... error monitoring makes this function really ugly...
    m_clPixels = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->pixelArraySize*sizeof(cl_char4), NULL, &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clCreateBuffer m_clPixels..." << endl;
    }
    m_clCubesMap = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->cubeMapArraySize*sizeof(cl_char), NULL, &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clCreateBuffer m_clCubesMap..." << endl;
    }
    m_clChildId = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->cubeMapArraySize*sizeof(cl_uint), NULL, &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clCreateBuffer m_clChildId..." << endl;
    }
    m_clCubeCorners = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->cubeMapArraySize*3*sizeof(cl_float4), NULL, &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clCreateBuffer m_clCubeCorners..." << endl;
    }

    //Allocate fixed size memory
    m_clObjectOffset = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY, sizeof(cl_int2), NULL, &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clCreateBuffer m_clObjectOffset..." << endl;
    }
    m_clNumberOfLevels = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY, sizeof(cl_uint), NULL, &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clCreateBuffer m_clNumberOfLevels..." << endl;
    }
    m_clTopCubeId = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY, sizeof(cl_uint), NULL, &error);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clCreateBuffer m_clTopCubeId..." << endl;
    }

    //Set Args
    error = clSetKernelArg(m_clRenderingKernel, 1, sizeof(m_clObjectOffset), &m_clObjectOffset);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clObjectOffset..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 2, sizeof(m_clPixels), &m_clPixels);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clPixels..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 3, sizeof(m_clCubesMap), &m_clCubesMap);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clCubesMap..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 4, sizeof(m_clChildId), &m_clChildId);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clChildId..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 5, sizeof(m_clCubeCorners), &m_clCubeCorners);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clCubeCorners..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 6, sizeof(m_clNumberOfLevels), &m_clNumberOfLevels);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clNumberOfLevels..." << endl;
    }
    error = clSetKernelArg(m_clRenderingKernel, 7, sizeof(m_clTopCubeId), &m_clTopCubeId);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clTopCubeId..." << endl;
    }

    error = clSetKernelArg(m_clClearKernel, 0, sizeof(m_clChildId), &m_clChildId);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- prepareOCL(): could not clSetKernelArg m_clChildId..." << endl;
    }

    //Allocate video memory
    if(m_i3cFile.isVideoFile()){
        //TODO: allocate video mem
    }

    //Upload data to GPU memory
    this->loadFrameOCL();
}

void GL_I3C_Element::loadFrameOCL()
{
    if(m_frame == NULL){
        return;
    }
    enqueueSetNumberOfLevels();
    if(m_i3cFile.isImageFile()){
        //|m_frame| should already be filled
        enqueueSetPixels();
        enqueueSetChildId();
        enqueueSetCubeMaps();
        enqueueSetTopCubeId();
    }
    else if(m_i3cFile.isVideoFile()){
        //TODO
    }

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
    m_clClearTextureKernel = NULL;      //DEBUG ONLY

    //Mem (Arguments)
    m_clCubeCorners = NULL;
    m_clPixels = NULL;
    m_clCubesMap = NULL;
    m_clChildId = NULL;
    m_clDiffPixels = NULL;
    m_clDiffCubesMap = NULL;
    m_clDiffChildId = NULL;
    m_clObjectOffset = NULL;
    m_clNumberOfLevels = NULL;
    m_clTopCubeId = NULL;
}

void GL_I3C_Element::releaseKernels()
{
    cl_int error;
    //Release Kernels
    if(m_clRenderingKernel != NULL){
        error = clReleaseKernel(m_clRenderingKernel);
        m_clRenderingKernel = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseKernels(): could not clReleaseKernel m_clRenderingKernel..." << endl;
        }
    }
    if(m_clClearKernel != NULL){
        error = clReleaseKernel(m_clClearKernel);
        m_clClearKernel = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseKernels(): could not clReleaseKernel m_clClearKernel..." << endl;
        }
    }
    if(m_clLoadVideoBufferKernel != NULL){
        error = clReleaseKernel(m_clLoadVideoBufferKernel);
        m_clLoadVideoBufferKernel = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseKernels(): could not clReleaseKernel m_clLoadVideoBufferKernel..." << endl;
        }
    }
    if(m_clClearTextureKernel != NULL){     //DEBUG ONLY
        error = clReleaseKernel(m_clClearTextureKernel);
        m_clClearTextureKernel = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseKernels(): could not clReleaseKernel m_clClearTextureKernel..." << endl;
        }
    }
}

void GL_I3C_Element::releaseArguments()
{
    cl_int error;
    //Release Argiments
    if(m_clCubeCorners != NULL){
        error = clReleaseMemObject(m_clCubeCorners);
        m_clCubeCorners = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clCubeCorners..." << endl;
        }
    }
    if(m_clPixels != NULL){
        error = clReleaseMemObject(m_clPixels);
        m_clPixels = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clPixels..." << endl;
        }
    }
    if(m_clCubesMap != NULL){
        error = clReleaseMemObject(m_clCubesMap);
        m_clCubesMap = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clCubesMap..." << endl;
        }
    }
    if(m_clChildId != NULL){
        error = clReleaseMemObject(m_clChildId);
        m_clChildId = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clChildId..." << endl;
        }
    }
    if(m_clDiffPixels != NULL){
        error = clReleaseMemObject(m_clDiffPixels);
        m_clDiffPixels = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clDiffPixels..." << endl;
        }
    }
    if(m_clDiffCubesMap != NULL){
        error = clReleaseMemObject(m_clDiffCubesMap);
        m_clDiffCubesMap = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clDiffCubesMap..." << endl;
        }
    }
    if(m_clDiffChildId != NULL){
        error = clReleaseMemObject(m_clDiffChildId);
        m_clDiffChildId = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clDiffChildId..." << endl;
        }
    }
    if(m_clObjectOffset != NULL){
        error = clReleaseMemObject(m_clObjectOffset);
        m_clObjectOffset = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clObjectOffset..." << endl;
        }
    }
    if(m_clNumberOfLevels != NULL){
        error = clReleaseMemObject(m_clNumberOfLevels);
        m_clNumberOfLevels = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clNumberOfLevels..." << endl;
        }
    }
    if(m_clTopCubeId != NULL){
        error = clReleaseMemObject(m_clTopCubeId);
        m_clTopCubeId = NULL;
        if(error != CL_SUCCESS){
            logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseArguments(): could not ReleaseMemObject m_clTopCubeId..." << endl;
        }
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

void GL_I3C_Element::acquireGLTexture()
{
    glFinish(); //FIXME: IMPROVE THAT...
    cl_int error;
    error = clEnqueueAcquireGLObjects(*m_clQueue, 1, m_clRenderingTexture, 0, 0, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- acquireGLTexture(): could not acquireGLTexture..." << endl;
        exit(-1);
    }
    error = clEnqueueAcquireGLObjects(*m_clQueue, 1, m_clDepthMap, 0, 0, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- acquireGLTexture(): could not acquire depthmap..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::releaseGLTexture()
{
    cl_int error;
    error = clFinish(*m_clQueue);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseGLTexture(): could not clFinish..." << endl;
    }
    error = clEnqueueReleaseGLObjects(*m_clQueue, 1, m_clRenderingTexture, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseGLTexture(): could not releaseGLTexture..." << endl;
        exit(-1);
    }
    error = clEnqueueReleaseGLObjects(*m_clQueue, 1, m_clDepthMap, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- releaseGLTexture(): could not release depthmap..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueSetScreenBoundaries()
{
    //Update Object Boundaries
    cl_int2 objectScreenOffset;
    objectScreenOffset.s[0] = (cl_int)m_ObjectBoundOnScreen.x;
    objectScreenOffset.s[1] = (cl_int)m_ObjectBoundOnScreen.y;
    cl_int error = clEnqueueWriteBuffer(*m_clQueue, m_clObjectOffset, CL_TRUE, 0, sizeof(objectScreenOffset),
                         &objectScreenOffset, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueSetScreenBoundaries(): could not enqueueSetScreenBoundaries..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueSetNumberOfLevels()
{
    cl_uint numberOfLevels;
    numberOfLevels = (cl_uint)m_frame->numberOfLevels;
    cl_int error = clEnqueueWriteBuffer(*m_clQueue, m_clNumberOfLevels, CL_TRUE, 0, sizeof(numberOfLevels),
                         &numberOfLevels, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueSetNumberOfLevels(): could not enqueueSetNumberOfLevels..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueSetTopCubeId()
{
    cl_uint topCubeId;
    topCubeId = (cl_uint)m_frame->cubeMapArraySize-1;
    cl_int error = clEnqueueWriteBuffer(*m_clQueue, m_clTopCubeId, CL_TRUE, 0, sizeof(topCubeId),
                         &topCubeId, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueSetTopCubeId(): could not enqueueSetTopCubeId..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueSetCubeCorners()
{
    cl_float4 cubeCorners[3];
    int cornerIndex[4] = {0, 2, 3, 6}; // See diagram: we only store 3 point for a given cube on GPU
    for(int i = 0; i < 3; i++){
        cubeCorners[i].s[0] = (cl_float)m_transformedObject.x[cornerIndex[i]];
        cubeCorners[i].s[1] = (cl_float)m_transformedObject.y[cornerIndex[i]];
        cubeCorners[i].s[2] = (cl_float)m_transformedObject.z[cornerIndex[i]];
    }
    cubeCorners[0].s[3] = (cl_float)m_transformedObject.x[cornerIndex[3]];
    cubeCorners[1].s[3] = (cl_float)m_transformedObject.y[cornerIndex[3]];
    cubeCorners[2].s[3] = (cl_float)m_transformedObject.z[cornerIndex[3]];

    cl_int error = clEnqueueWriteBuffer(*m_clQueue, m_clCubeCorners, CL_TRUE, (m_frame->cubeMapArraySize-1) * sizeof(cubeCorners),
                                        sizeof(cubeCorners), cubeCorners, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueSetCubeCorners(): could not enqueueSetCubeCorners..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueSetPixels()
{
    // Loads pixels form m_frame to OCL memory
    int numberOfPixels = m_frame->pixelArraySize;
    cl_uchar4 *pixels = new cl_uchar4[numberOfPixels];
    for(int i = 0; i < numberOfPixels; i++){
        pixels[i].s[0] = m_frame->pixel[i].red;
        pixels[i].s[1] = m_frame->pixel[i].green;
        pixels[i].s[2] = m_frame->pixel[i].blue;
        pixels[i].s[3] = 255;   // Alpha
        //cout << m_frame->pixel[i].red << endl;
    }
    cl_int error = clEnqueueWriteBuffer(*m_clQueue, m_clPixels, CL_TRUE, 0, numberOfPixels*sizeof(cl_uchar4),
                         pixels, 0, NULL, NULL);
    delete[] pixels;

    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueSetPixels(): could not enqueueSetPixels..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueSetChildId()
{
    int numberOfChildId = m_frame->cubeMapArraySize;
    cl_uint *childId = new cl_uint[numberOfChildId];
    for(int i = 0; i < numberOfChildId; i++){
        childId[i] = m_frame->childCubeId[i];
    }
    cl_int error = clEnqueueWriteBuffer(*m_clQueue, m_clChildId, CL_TRUE, 0, numberOfChildId*sizeof(cl_uint),
                         childId, 0, NULL, NULL);
    delete[] childId;
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueSetChildId(): could not enqueueSetChildId..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueSetCubeMaps()
{
    int numberOfMaps = m_frame->cubeMapArraySize;
    cl_uchar *maps = new cl_uchar[numberOfMaps];
    for(int i = 0; i < numberOfMaps; i++){
        maps[i] = m_frame->cubeMap[i];
    }
    cl_int error = clEnqueueWriteBuffer(*m_clQueue, m_clCubesMap, CL_TRUE, 0, numberOfMaps*sizeof(cl_uchar),
                         maps, 0, NULL, NULL);
    delete[] maps;

    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueSetCubeMaps(): could not enqueueSetCubeMaps..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueClearMemoryBit()
{
    size_t wi_clear[1] = {m_frame->cubeMapArraySize};
    cl_int error = clEnqueueNDRangeKernel(*m_clQueue, m_clClearKernel, 1, NULL,
                                          wi_clear , NULL, 0, NULL, NULL);

    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueClearMemoryBit(): could not enqueueClearMemoryBit..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueClearTexture()
{
    size_t wi_clear[2] = {m_screenWidth, m_screenHeight};
    cl_int error = clEnqueueNDRangeKernel(*m_clQueue, m_clClearTextureKernel, 2, NULL,
                                          wi_clear , NULL, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueClearTexture(): could not enqueueClearTexture..." << endl;
        exit(-1);
    }
}

void GL_I3C_Element::enqueueRender()
{
    //Create the appropriate amount of workitems (w and h from m_ObjectBoundOnScreen)
    size_t wi[2] = {m_ObjectBoundOnScreen.w, m_ObjectBoundOnScreen.h};
    cl_int error = clEnqueueNDRangeKernel(*m_clQueue, m_clRenderingKernel, 2, NULL,
                                          wi , NULL, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        logs << "i3c_error : " << error <<": GL_I3C_Element --- enqueueRender(): could not enqueueRender..." << endl;
        exit(-1);
    }
}
