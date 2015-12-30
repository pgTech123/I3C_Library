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
    m_clClearTextureKernel = clCreateKernel(*program, "clearTexture", NULL);    //DEBUG ONLY
    m_clClearKernel = clCreateKernel(*program, "clearMemoryBit", NULL);
    m_clLoadVideoBufferKernel = clCreateKernel(*program, "loadVideoBuffer", NULL);
    //TODO: Create other kernels here

    //If a file was already specified, load it
    if(m_fileLoaded){
        this->prepareOCL();
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
    clSetKernelArg(m_clClearTextureKernel, 0, sizeof(*m_clRenderingTexture), m_clRenderingTexture); //DEBUG ONLY

    m_texturesSpecified = true;
}

void GL_I3C_Element::render()
{
    if(m_texturesSpecified){
        //Compute object transform
        computeTransform(&m_originalCoord, &m_transformedObject, m_transformMatrix);
        projectObject(&m_transformedObject, &m_ObjectBoundOnScreen, m_screenWidth, m_screenHeight);

        this->acquireGLTexture();

        //IF position exactly = to last position, do not clear precomputed cube positions

        //Clear previous texture
        this->enqueueClearTexture();

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
    if(m_frame == NULL){
        return;
    }

    //Get the space needed
    m_i3cFile.read(m_frame);

    //Clear mem previously allocated
    this->releaseArguments();

    //Allocate memory
    m_clPixels = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->pixelArraySize*sizeof(cl_char4), NULL, NULL);
    m_clCubesMap = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->cubeMapArraySize*sizeof(cl_char), NULL, NULL);
    m_clChildId = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->cubeMapArraySize*sizeof(cl_uint), NULL, NULL);
    m_clCubeCorners = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY,
                                   m_frame->cubeMapArraySize*3*sizeof(cl_float4), NULL, NULL);

    //Allocate fixed size memory
    m_clObjectOffset = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY, sizeof(cl_int2), NULL, NULL);
    m_clNumberOfLevels = clCreateBuffer(*m_clContext, CL_MEM_READ_ONLY, sizeof(cl_uint), NULL, NULL);

    //Set Args
    clSetKernelArg(m_clRenderingKernel, 1, sizeof(m_clObjectOffset), &m_clObjectOffset);
    clSetKernelArg(m_clRenderingKernel, 2, sizeof(m_clPixels), &m_clPixels);
    clSetKernelArg(m_clRenderingKernel, 3, sizeof(m_clCubesMap), &m_clCubesMap);
    clSetKernelArg(m_clRenderingKernel, 4, sizeof(m_clChildId), &m_clChildId);
    clSetKernelArg(m_clRenderingKernel, 5, sizeof(m_clCubeCorners), &m_clCubeCorners);
    clSetKernelArg(m_clRenderingKernel, 6, sizeof(m_clNumberOfLevels), &m_clNumberOfLevels);

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
    if(m_clClearTextureKernel != NULL){     //DEBUG ONLY
        clReleaseKernel(m_clClearTextureKernel);
        m_clClearTextureKernel = NULL;
    }
}

void GL_I3C_Element::releaseArguments()
{
    //Release Argiments
    if(m_clCubeCorners != NULL){
        clReleaseMemObject(m_clCubeCorners);
        m_clCubeCorners = NULL;
    }
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
    if(m_clNumberOfLevels != NULL){
        clReleaseMemObject(m_clNumberOfLevels);
        m_clNumberOfLevels = NULL;
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
    cl_int error = clEnqueueAcquireGLObjects(*m_clQueue, 1, m_clRenderingTexture, 0, 0, NULL);
    if(error != CL_SUCCESS){
        cout << "Error aquiring the texture..." << endl;
    }
}

void GL_I3C_Element::releaseGLTexture()
{
    clFinish(*m_clQueue);
    cl_int error = clEnqueueReleaseGLObjects(*m_clQueue, 1, m_clRenderingTexture, 0, 0, NULL);
    if(error != CL_SUCCESS){
        cout << "Error releasing texture..." << endl;
    }
}

void GL_I3C_Element::enqueueSetScreenBoundaries()
{
    //Update Object Boundaries
    cl_int2 objectScreenOffset;
    objectScreenOffset.s[0] = (cl_int)m_ObjectBoundOnScreen.x;
    objectScreenOffset.s[1] = (cl_int)m_ObjectBoundOnScreen.y;
    clEnqueueWriteBuffer(*m_clQueue, m_clObjectOffset, CL_TRUE, 0, sizeof(objectScreenOffset),
                         &objectScreenOffset, 0, NULL, NULL);
}

void GL_I3C_Element::enqueueSetNumberOfLevels()
{
    cl_uint numberOfLevels;
    numberOfLevels = (cl_uint)m_frame->numberOfLevels;
    clEnqueueWriteBuffer(*m_clQueue, m_clNumberOfLevels, CL_TRUE, 0, sizeof(numberOfLevels),
                         &numberOfLevels, 0, NULL, NULL);
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

    clEnqueueWriteBuffer(*m_clQueue, m_clCubeCorners, CL_TRUE, 0, sizeof(cubeCorners),
                         cubeCorners, 0, NULL, NULL);
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
    clEnqueueWriteBuffer(*m_clQueue, m_clPixels, CL_TRUE, 0, numberOfPixels*sizeof(cl_uchar4),
                         pixels, 0, NULL, NULL);
    delete[] pixels;
}

void GL_I3C_Element::enqueueSetChildId()
{
    int numberOfChildId = m_frame->cubeMapArraySize;
    cl_uint *childId = new cl_uint[numberOfChildId];
    for(int i = 0; i < numberOfChildId; i++){
        childId[i] = m_frame->childCubeId[i];
    }
    cout << childId[0] << endl;
    clEnqueueWriteBuffer(*m_clQueue, m_clChildId, CL_TRUE, 0, numberOfChildId*sizeof(cl_uint),
                         childId, 0, NULL, NULL);
    delete[] childId;
}

void GL_I3C_Element::enqueueSetCubeMaps()
{
    int numberOfMaps = m_frame->cubeMapArraySize;
    cl_uchar *maps = new cl_uchar[numberOfMaps];
    for(int i = 0; i < numberOfMaps; i++){
        maps[i] = m_frame->cubeMap[i];
    }
    clEnqueueWriteBuffer(*m_clQueue, m_clCubesMap, CL_TRUE, 0, numberOfMaps*sizeof(cl_uchar),
                         maps, 0, NULL, NULL);
    delete[] maps;
}

void GL_I3C_Element::enqueueClearTexture()
{
    size_t wi_clear[2] = {m_screenWidth, m_screenHeight};
    cl_int error = clEnqueueNDRangeKernel(*m_clQueue, m_clClearTextureKernel, 2, NULL,
                                          wi_clear , NULL, 0, NULL, NULL);

    if(error != CL_SUCCESS){
        cout << "Error Clearing the texture" << endl;
    }
}

void GL_I3C_Element::enqueueRender()
{
    //Create the appropriate amount of workitems (w and h from m_ObjectBoundOnScreen)
    size_t wi[2] = {m_ObjectBoundOnScreen.w, m_ObjectBoundOnScreen.h};
    cl_int error = clEnqueueNDRangeKernel(*m_clQueue, m_clRenderingKernel, 2, NULL,
                                          wi , NULL, 0, NULL, NULL);
    if(error == CL_INVALID_KERNEL_ARGS){
        cout << "Invalid Arg" << endl;
    }
    else if(error != CL_SUCCESS){
        cout << "Error Rendering" << endl;
    }
}
