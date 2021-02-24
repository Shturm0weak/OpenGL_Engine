#pragma once
#include "glew.h"
#include "glfw3.h"
#include <iostream>

#define NAMECOLOR "\033[1m\033[36m"

#define MAX_PREALLOCATED_INSTANCES 1000

#ifdef DOOM_ENGINE
#define DOOM_API __declspec(dllexport)
#else
#define DOOM_API __declspec(dllimport)
#endif

static int g_Height = 1080;
static int g_Width = 1080;

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