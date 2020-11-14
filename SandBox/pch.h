#ifndef PCH_H
#define PCH_H
#include "Core/Core.h"
#pragma warning(push)
#pragma warning(disable:4251)
#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <functional>
#include <limits>
#include <math.h>

#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

extern "C"
{
#include "lua.hpp"
#include "lauxlib.h"
#include "lualib.h"
}

#pragma comment(lib, "liblua54.a")

#include "Core/DeltaTime.h"
#include "Core/ColorsRGBA.h"
#include "Render/Shader.h"
#include "Render/IndexBuffer.h"
#include "Render/VertexBuffer.h"
#include "Render/VertexBufferLayout.h"
#include "Render/VertexArray.h"
#include "Render/Texture.h"
#include "Objects2D/GameObject.h"
#include <Windows.h>
#endif //PCH_H
