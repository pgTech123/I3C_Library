#include "basic_gl_i3c_rendertest.h"

Basic_GL_I3C_RenderTest::Basic_GL_I3C_RenderTest()
{
}

void Basic_GL_I3C_RenderTest::readI3CFile(const char* filename)
{
    //Open the file
    int error = m_i3c_reader.open(filename);
    if(error != I3C_SUCCESS){
        fprintf(m_p_errorFile, "Could not open file %s", filename);
    }

    //Read the file
    error = m_i3c_reader.read(&m_i3c_frame);
    if(error != I3C_SUCCESS){
        fprintf(m_p_errorFile, "An error occured while reading %s. The file might be corrupted.", filename);
    }

    //Close the file
    m_i3c_reader.close();
}

void Basic_GL_I3C_RenderTest::initGL()
{
    m_glProgram = compileShaders();
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);
}

void Basic_GL_I3C_RenderTest::renderGL()
{
    //Background color
    const GLfloat color[] = {0.0f, 0.2f, 0.0f, 1.0f};

    //Run GL program
    glClearBufferfv(GL_COLOR, 0, color);
    glUseProgram(m_glProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(m_gl_Window);
}


void Basic_GL_I3C_RenderTest::stopGL()
{
    //Clear memory
    glDeleteVertexArrays(1, &m_vertexArrayObject);
    glDeleteProgram(m_glProgram);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

GLuint Basic_GL_I3C_RenderTest::compileShaders()
{

    static const GLchar* vertex_shader_sources_basic_test[] =
    {
        "#version 130                                                           \n"
        "                                                                       \n"
        "void main(void)                                                        \n"
        "{                                                                      \n"
        "   const vec4 vertices[3] = vec4[3](vec4( 0.25, -0.25, 0.5, 1.0),      \n"
        "                                    vec4(-0.25, -0.25, 0.5, 1.0),      \n"
        "                                    vec4( 0.25,  0.25, 0.5, 1.0));     \n"
        "                                                                       \n"
        "   gl_Position = vertices[gl_VertexID];                                \n"
        "}                                                                      \n"
    };


    static const GLchar* fragment_shader_sources_basic_test[] =
    {
        "#version 130                                   \n"
        "                                               \n"
        "out vec4 color;                                \n"
        "                                               \n"
        "void main(void)                                \n"
        "{                                              \n"
        "   color = vec4(1.0, 1.0, 0.0, 1.0);           \n"
        "}                                              \n"
    };


    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

    //Create and compile vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, vertex_shader_sources_basic_test, NULL);
    glCompileShader(vertexShader);

    //Print compilation errors
    if(DEBUG){
        printShaderCompilationErrors(vertexShader);
    }

    //Create and compile framgent shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, fragment_shader_sources_basic_test, NULL);
    glCompileShader(fragmentShader);

    //Print compilation errors
    if(DEBUG){
        printShaderCompilationErrors(fragmentShader);
    }

    //Create program & linking
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    //Clear what's not needed anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Basic_GL_I3C_RenderTest::printShaderCompilationErrors(GLuint shader)
{
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE){
        cout << "vertex compilation error..." << endl;

        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        for (int i = 0; i < maxLength; i++){
            cout << errorLog[i];
        }
        cout << endl;
    }
}
