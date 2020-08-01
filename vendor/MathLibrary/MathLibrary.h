#pragma once

#ifdef MATH_LIBRARY
#define MATH_API __declspec(dllexport)
#else
#define MATH_API __declspec(dllimport)
#endif