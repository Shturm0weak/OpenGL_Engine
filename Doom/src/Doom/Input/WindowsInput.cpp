#include "../pch.h"
#include "WindowsInput.h"
#include "../Core/Window.h"
#include "../Core/ViewPort.h"

using namespace Doom;

//void Doom::WindowsInput::Clearimp()
//{
//	pressedMouseButtons.clear();
//	pressedKeysButtons.clear();
//}

bool WindowsInput::IsKeyPressedimp(int keycode)
{
	if (!ViewPort::GetInstance()->m_IsActive)
		return GL_FALSE;
	ImGui::SetCurrentContext(Window::GetInstance().s_ImGuiContext);
	return ImGui::IsKeyPressed(keycode);
	/*auto iter = pressedKeysButtons.find(keycode);
	if (iter != pressedKeysButtons.end()) {
		return iter->second;
	}

	double time = glfwGetTime();
	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());
	if (!ViewPort::GetInstance()->IsActive)
		return GL_FALSE;
	auto state = glfwGetKey(window, keycode);
	if (state == GLFW_PRESS) {
		auto iter = pressedButtonsTime.find(keycode);
		if (iter == pressedButtonsTime.end()) {
			pressedButtonsTime.insert(std::make_pair(keycode, time));
			pressedKeysButtons.insert(std::make_pair(keycode, true));
			return true;
		}

		if (time - iter->second < DeltaTime::deltatime * 4) {
			iter->second = time;
			pressedButtonsTime.insert(std::make_pair(keycode, time));
			pressedKeysButtons.insert(std::make_pair(keycode, false));
			return false;
		}

		iter->second = glfwGetTime();
		pressedKeysButtons.insert(std::make_pair(keycode, true));
		return true;
	}
	else {
		pressedKeysButtons.insert(std::make_pair(keycode, false));
		return false;
	}*/
}

bool WindowsInput::IsMousePressedDownimp(int keycode)
{
	if (!ViewPort::GetInstance()->m_IsHovered
		&& (ImGui::IsAnyWindowHovered()
			|| ImGui::IsAnyItemHovered()
			|| ImGui::IsAnyItemActive() 
			|| ImGui::IsAnyWindowFocused())) return GLFW_FALSE;

	const auto& window = static_cast<GLFWwindow*>(Window::GetInstance().GetWindow());
	if (ViewPort::GetInstance()->m_IsHovered)
	{
		auto state = glfwGetMouseButton(window, keycode);
		return state == GLFW_PRESS;
	}
	return false;
	/*const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());

	if (!ViewPort::GetInstance()->IsHovered && (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyWindowFocused())) {
		return GLFW_FALSE;
	}

	if (ViewPort::GetInstance()->IsHovered) {
		auto state = glfwGetMouseButton(window, keycode);
		return state == GLFW_PRESS;
	}*/
}

bool Doom::WindowsInput::IsKeyPressedDownimp(int keycode)
{
	if (!ViewPort::GetInstance()->m_IsActive) return GL_FALSE;
	ImGui::SetCurrentContext(Window::GetInstance().s_ImGuiContext);
	return ImGui::IsKeyDown(keycode);
	/*const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());
	if (!ViewPort::GetInstance()->IsActive)
		return GL_FALSE;
	auto state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS;*/
}

bool WindowsInput::IsMousePressedimp(int keycode)
{
	if (!ViewPort::GetInstance()->m_IsHovered
		&& (ImGui::IsAnyWindowHovered()
			|| ImGui::IsAnyItemHovered() 
			|| ImGui::IsAnyItemActive() 
			|| ImGui::IsAnyWindowFocused())) return GLFW_FALSE;
	if (ViewPort::GetInstance()->m_IsHovered) 
	{
		ImGui::SetCurrentContext(Window::GetInstance().s_ImGuiContext);
		return ImGui::IsMouseClicked(keycode);
	}
	return false;
	/*auto iter = pressedMouseButtons.find(keycode);
	if (iter != pressedMouseButtons.end()) {
		return iter->second;
	}

	double time = glfwGetTime();

	const auto& window = static_cast<GLFWwindow*>(Window::GetWindow());
	
	if (!ViewPort::GetInstance()->IsHovered && (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyWindowFocused())) {
		return GLFW_FALSE;
	}
	
	if (ViewPort::GetInstance()->IsHovered) {
		auto state = glfwGetMouseButton(window, keycode);
		if (state == GLFW_PRESS) {
			auto iter = pressedButtonsTime.find(keycode);
			if (iter == pressedButtonsTime.end()) {
				pressedButtonsTime.insert(std::make_pair(keycode, time));
				pressedMouseButtons.insert(std::make_pair(keycode, true));
				return true;
			}

			if (time - iter->second < DeltaTime::deltatime * 3) {
				iter->second = time;
				pressedMouseButtons.insert(std::make_pair(keycode, false));
				return false;
			}
			iter->second = glfwGetTime();
			pressedMouseButtons.insert(std::make_pair(keycode, true));
			return true;
		}
		else {
			return false;
			pressedMouseButtons.insert(std::make_pair(keycode, false));
		}
	}*/
}
