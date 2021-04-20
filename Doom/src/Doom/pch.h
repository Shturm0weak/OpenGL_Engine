#ifndef PCH_H
#define PCH_H

#include "Core/Core.h"

#pragma warning(push)

#pragma warning(disable:4251)
#pragma warning(disable:4244)
#pragma warning(disable:4267)

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <functional>
#include <limits>
#include <math.h>
#include <ctime>    

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
#include "Enums/ColorsRGBA.h"
#include "OpenGl/Shader.h"
#include "OpenGl/IndexBuffer.h"
#include "OpenGl/VertexBuffer.h"
#include "OpenGl/VertexBufferLayout.h"
#include "OpenGl/VertexArray.h"
#include "OpenGl/Texture.h"
#include "Objects/GameObject.h"
#include "Core/Timer.h"
#include "FileDialogs/FileDialogs.h"
#include "Core/World.h"
#include "Core/Logger.h"
#endif // !PCH_H