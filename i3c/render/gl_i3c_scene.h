//Author:           Pascal Gendron
//Creation date:    September 11th 2015
//Description:

#ifndef GL_I3C_SCENE_H
#define GL_I3C_SCENE_H

#ifdef __APPLE__            //APPLE OS HAS NOT BEEN TESTED YET
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32               //WINDOWS OS HAS NOT BEEN TESTED YET
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "gl_i3c_element.h"

class GL_I3C_Scene
{
public:
    GL_I3C_Scene();
    ~GL_I3C_Scene();

    void attachGLContext(); //Adds shaders for determination of I3C regions

    void addI3CElement(GL_I3C_Element* element);
    void retreiveI3CElement(GL_I3C_Element* element);
    void retreiveAll();

    void renderI3C();       //Render I3C elements in the visible regions

private:

};

#endif // GL_I3C_SCENE_H
