#include "gl_i3c_scene.h"

GL_I3C_Scene::GL_I3C_Scene()
{
    this->initOCL();
}

GL_I3C_Scene::~GL_I3C_Scene()
{
    this->shutdownOCL();
}

void GL_I3C_Scene::attachGLContext(HDC hDC, HGLRC hRC)
{
    //Create OpenCL context
    int error = this->createOCLContext(hDC, hRC);
}

bool GL_I3C_Scene::isUpAndRunning()
{
    return m_OCL_OK;
}

bool GL_I3C_Scene::setRenderingTextures(GLuint renderingTexture, GLuint depthMap)
{
    if(m_OCL_OK){
        //Release previous texture (if any)
        this->releaseTextures();

        //Update with new texture
        m_clRenderingTexture = clCreateFromGLTexture(m_context, CL_MEM_READ_WRITE, GL_TEXTURE_2D,
                                                     0, renderingTexture, NULL);
        m_clDepthMap = clCreateFromGLTexture(m_context, CL_MEM_READ_WRITE, GL_TEXTURE_2D,
                                             0, depthMap, NULL);

        //Update Screen Size
        glBindTexture(GL_TEXTURE_2D, renderingTexture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_ScreenWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_ScreenHeight);

        //DEBUG
        //cout << m_ScreenWidth << ", " << m_ScreenHeight << endl;

        //Apply to children
        int i3cElementsSize = m_i3cElements.size();
        for(int i = 0; i < i3cElementsSize; i++)
        {
            m_i3cElements.at(i)->setScreenSize(m_ScreenWidth, m_ScreenHeight);
            m_i3cElements.at(i)->setTextures(&m_clRenderingTexture, &m_clDepthMap);
        }

        return true;
    }
    return false;
}

void GL_I3C_Scene::stopRender()
{
    glFinish();
    clFinish(m_queue);
}

bool GL_I3C_Scene::addI3CElement(GL_I3C_Element* element)
{
    //Look if not already there
    int i3cElementsSize = m_i3cElements.size();
    for(int i = 0; i < i3cElementsSize; i++)
    {
        //If the pointer match, return false: (already there)
        if(m_i3cElements.at(i) == element){
            return false;
        }
    }

    //Set OCL context
    if(!element->setOCLContext(&m_context, &m_program, &m_queue)){
        return false;
    }

    //Set rendering textures
    element->setTextures(&m_clRenderingTexture, &m_clDepthMap);
    element->setViewTransformMatrix(&m_transformMatrix);
    element->setScreenSize(m_ScreenWidth, m_ScreenHeight);
    m_i3cElements.push_back(element);   //Add the element to the vector

    return true;
}

void GL_I3C_Scene::retreiveI3CElement(GL_I3C_Element* element)
{
    //Look at all the known i3c elements and remove the ones that match
    int i3cElementsSize = m_i3cElements.size();
    for(int i = 0; i < i3cElementsSize; i++)
    {
        //If the pointer match, remove the element
        if(m_i3cElements.at(i) == element){
            m_i3cElements[i]->resetOCLContext();    //Reset OCL context
            m_i3cElements.erase(m_i3cElements.begin()+i);
        }
    }
}

void GL_I3C_Scene::retreiveAll()
{
    //Reset OCL context
    int i3cElementsSize = m_i3cElements.size();
    for(int i = 0; i < i3cElementsSize; i++){
        m_i3cElements[i]->resetOCLContext();
    }

    //Remove all known i3c elements
    m_i3cElements.clear();
}

void GL_I3C_Scene::lookAt(Position position, Axis rotation)
{
    matFromPosAndRot(&m_transformMatrix, position, rotation);
}

void GL_I3C_Scene::lookAt(float x, float y, float z, float pitch, float yaw, float roll)
{
    Position pos;
    Axis rot;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    rot.x  = pitch;
    rot.y = yaw;
    rot.z = roll;

    this->lookAt(pos, rot);
}

void GL_I3C_Scene::setTransformMatrix(Mat4x4 transformMat)
{
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            m_transformMatrix.element[i][j] = transformMat.element[i][j];
        }
    }
}

void GL_I3C_Scene::renderI3C()
{
    int i3cElementsSize = m_i3cElements.size();
    for(int i = 0; i < i3cElementsSize; i++)
    {
        m_i3cElements[i]->render();
    }
}

void GL_I3C_Scene::initOCL()
{
    m_OCL_OK = false;

    m_device = NULL;
    m_context = NULL;
    m_queue = NULL;
    m_program = NULL;

    m_clRenderingTexture = NULL;
    m_clDepthMap = NULL;
    this->initTransformMat();

}

void GL_I3C_Scene::shutdownOCL()
{
    //As elements use OCl context, we retreive them first
    this->retreiveAll();

    this->releaseTextures();

    if(m_program != NULL){
        clReleaseProgram(m_program);
        m_program = NULL;
    }

    //Release everything that has been initialized
    if(m_queue != NULL){
        clReleaseCommandQueue(m_queue);
        m_queue = NULL;
    }
    if(m_context != NULL){
        clReleaseContext(m_context);
        m_context = NULL;
    }
    if(m_device != NULL){
        clReleaseDevice(m_device);
        m_device = NULL;
    }

    m_OCL_OK = false;
}

void GL_I3C_Scene::releaseTextures()
{
    //Release textures
    if(m_clRenderingTexture != NULL){
        clReleaseMemObject(m_clRenderingTexture);
        m_clRenderingTexture = NULL;
    }
    if(m_clDepthMap != NULL){
        clReleaseMemObject(m_clDepthMap);
        m_clDepthMap = NULL;
    }
}

void GL_I3C_Scene::initTransformMat()
{
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            m_transformMatrix.element[i][j] = 0;
        }
    }
}

int GL_I3C_Scene::createOCLContext(HDC hDC, HGLRC hRC)
{
    cl_int status;
    cl_uint platformCount = 0;
    cl_platform_id platform[MAX_NUMBER_OF_PLATFORM];
    clGetGLContextInfoKHR_fn _clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");

    //Count platforms available
    status = clGetPlatformIDs(0, NULL, &platformCount);

    //Get platform(s)
    clGetPlatformIDs(platformCount, platform, NULL);

    //Try to find on witch platform OpenGL is running
    for (cl_uint i=0; i < platformCount; i++)
    {
        cl_context_properties GL_context_prop[] =
        {
            CL_GL_CONTEXT_KHR,   (cl_context_properties)hRC,
            CL_WGL_HDC_KHR,      (cl_context_properties)hDC,
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform[i],
            0
        };

        //FIXME: Possibility of improvements here: multiple devices
        //Find on which platform the OpenGL context is running and init OpenCL there.
        status = _clGetGLContextInfoKHR(GL_context_prop, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id), &m_device,  NULL);
        if(status == CL_SUCCESS){
            m_context = clCreateContext(GL_context_prop, 1, &m_device, 0, 0, NULL);
            m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, NULL);
            this->loadOCLProgram();

            m_OCL_OK = true;
            return I3C_SUCCESS;
        }
    }

    //If we get here something wrong happened
    return I3C_CANNOT_FIND_CONTEXT;
}

void GL_I3C_Scene::loadOCLProgram()
{
    cl_int error;

    //Load sources
    m_clSources = NULL;
    loadOCLSources();

    m_program = clCreateProgramWithSource(m_context, 1, (const char **)&m_clSources,
                                          (const size_t *)MAX_FILE_SIZE, &error);
    if(error != CL_SUCCESS){
        cerr << "Error Opencl Sources: code " << error << endl;
    }
    error = clBuildProgram(m_program, 1, &m_device, NULL, NULL, NULL);

    //Make sure the sources doesn't cause a mem leak
    if(m_clSources != NULL){
        delete[] m_clSources;
        m_clSources = NULL;
    }

    if(error != CL_SUCCESS){
        char *buff_erro;
        cl_int errcode;
        size_t build_log_len;
        errcode = clGetProgramBuildInfo(m_program, m_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
        if (errcode) {
            cout << "clGetProgramBuildInfo failed at line " << __LINE__ << endl;
            exit(-1);
        }

        buff_erro = new char[build_log_len];
        if (!buff_erro) {
            cout << "Error allocating build log array..." << __LINE__ << endl;
            exit(-2);
        }

        errcode = clGetProgramBuildInfo(m_program, m_device, CL_PROGRAM_BUILD_LOG, build_log_len, buff_erro, NULL);
        if (errcode) {
            cout << "clGetProgramBuildInfo failed at line " << __LINE__ << endl;
            exit(-3);
        }

        cout << " --- Build log: ---" << endl;
        cerr << buff_erro << endl; //Be careful with  the fprint
        delete[] buff_erro;
        cout << endl << "clBuildProgram failed" << endl;
        exit(EXIT_FAILURE);
    }
}

void GL_I3C_Scene::loadOCLSources()
{
    //Load and append all sources in a char array
    ostringstream oss;
    for(int i = 0; i < NUM_OF_FILES_TO_LOAD; i++)
    {
        ifstream kernelFile(FILES_TO_LOAD[i], ios::in);
        if (!kernelFile.is_open()){
            cerr << "Failed to open file for reading: " << FILES_TO_LOAD[i] << endl;
            cerr << "Abort..." << endl;
            exit(EXIT_FAILURE);
            return;
        }
        oss << kernelFile.rdbuf();
    }

    m_clSources = strdup(oss.str().c_str());
}
