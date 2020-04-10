#include "../pch.h"
#include "WindowsInput.h"
#include "Window.h"

using namespace Doom;

Input* Input::s_Instance = new WindowsInput();;

bool WindowsInput::IsKeyPressedimp(int keycode)
{
	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());

	auto state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS;
}

bool WindowsInput::IsMousePressedimp(int keycode) {
	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());

	auto state = glfwGetMouseButton(window, keycode);
	return state == GLFW_PRESS;
}
