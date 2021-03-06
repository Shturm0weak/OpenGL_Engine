#pragma once
#include "glew.h"
#include "glfw3.h"
#include <iostream>

#define NAMECOLOR "\033[1m\033[36m"

#define MAX_PREALLOCATED_INSTANCES 10000

#ifdef DOOM_ENGINE
#define DOOM_API __declspec(dllexport)
#else
#define DOOM_API __declspec(dllimport)
#endif

static int g_ScaleUI = 1000;

static void GlClearError() 
{
	while (glGetError() != 0);
}

static void GlCheckError() {
	while (GLenum error = glGetError()) 
	{
		std::cout << "Opengl error: " << error << std::endl;
	}
}