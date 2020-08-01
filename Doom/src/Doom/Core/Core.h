#pragma once
#include "glew.h"
#include "glfw3.h"
#include <iostream>

#ifdef DOOM_ENGINE
#define DOOM_API __declspec(dllexport)
#else
#define DOOM_API __declspec(dllimport)
#endif
static int HEIGHT = 1080;
static int WIDTH = 1080;

static void GlClearError() {
	while (glGetError() != 0);
}

static void GlCheckError() {
	while (GLenum error = glGetError()) {
		std::cout << "Opengl error: " << error << std::endl;
	}
}