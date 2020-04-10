#pragma once

#include "glew.h"
#include "glfw3.h"
#include "glfw3native.h"

#ifdef DOOM_ENGINE
#define DOOM_API __declspec(dllexport)
#else
#define DOOM_API __declspec(dllimport)
#endif

