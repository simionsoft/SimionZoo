// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#undef min
#undef max


//GLEW
#define GLEW_STATIC
#include "../../3rd-party/glew2/include/glew.h"


//GLUT
#define FREEGLUT_STATIC 1
#define FREEGLUT_LIB_PRAGMAS 0
#include "../../3rd-party/freeglut3/include/GL/freeglut.h"

#ifdef _WIN32
#pragma comment (lib,"opengl32.lib")
#endif

//tinyxml2
#include "../../3rd-party/tinyxml2/tinyxml2.h"
using namespace tinyxml2;

//SOIL
#include "../../3rd-party/SOIL/include/SOIL.h"