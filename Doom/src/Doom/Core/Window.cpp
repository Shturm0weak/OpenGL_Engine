#include "Window.h"
#include "ViewPort.h"

int* Doom::Window::GetSize()
{
	static int size[2];
	return size;
}

void Doom::Window::HideCursor()
{
	glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	s_CursorStateChanged = true;
}

void Doom::Window::ShowCursor()
{
	glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	s_CursorStateChanged = true;
	ViewPort::GetInstance().RecalculateMouseCoords();
}

void Doom::Window::SetCurrentContext(GLFWwindow* context)
{
	glfwMakeContextCurrent(context);
}

glm::dvec2 Doom::Window::GlfwGetMousePos()
{
	glm::dvec2 pos;
	glfwGetCursorPos(Window::GetWindow(), &pos.x, &pos.y);
	return pos;
}

int Doom::Window::Init(const char* Label, float width, float height, bool vsync)
{
	if (!glfwInit())
		return -1;
	GetSize()[0] = width;
	GetSize()[1] = height;

	s_Window = glfwCreateWindow(width, height, Label, NULL, NULL);
	if (!GetWindow())
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(GetWindow());

	if (glewInit() != GLEW_OK) 
	{
		Logger::Error("failed to initialize GLEW!");
		return -1;
	}

	glfwSwapInterval(vsync); // Enable vsync

	glfwSetScrollCallback(Window::GetWindow(), [](GLFWwindow* window, double xoffset, double yoffset) {
		Window::GetInstance().GetScrollYOffset() = yoffset;
		});

	glfwSetWindowSizeCallback(Window::GetWindow(), [](GLFWwindow* window, int width, int height) {
		Window& w = Window::GetInstance();
		int* size = w.GetSize();
		size[0] = width;
		size[1] = height;
		glViewport(0, 0, size[0], size[1]);
	});
	s_ImGuiContext = ImGui::CreateContext();
	s_ImGuiIO = &ImGui::GetIO();
	(void)s_ImGuiIO;
	ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGui::StyleColorsClassic();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	Logger::Success("Window has been Initialized!");

	return 0;
}

float Doom::Window::GetFPS()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	return ImGui::GetIO().Framerate;
}

void Doom::Window::DisableCursor()
{
	glfwSetInputMode(Window::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	s_CursorStateChanged = true;
}

int Doom::Window::GetCursorMode()
{
	return glfwGetInputMode(Window::GetWindow(), GLFW_CURSOR);
}

Doom::Window::~Window()
{
	delete m_FrameBufferBlur[0];
	delete m_FrameBufferBlur[1];
	delete m_FrameBufferColor;
	delete m_FrameBufferShadowMap;
}

Doom::Window& Doom::Window::GetInstance()
{
	static Window instance;
	return instance;
}

void Doom::Window::ClampCursorPos()
{
	glm::dvec2 mousePos = Window::GlfwGetMousePos();
	if (Window::GetCursorMode() == GLFW_CURSOR_DISABLED) {
		glm::dvec2 newCursorPos = mousePos;
		if (mousePos.x > Window::GetSize()[0])
			newCursorPos.x = 0;
		else if (mousePos.x < 0)
			newCursorPos.x = Window::GetSize()[0];
		if (mousePos.y > Window::GetSize()[1])
			newCursorPos.y = 0;
		else if (mousePos.y < 0)
			newCursorPos.y = Window::GetSize()[1];
		Window::SetCursorPos(newCursorPos);
		if (mousePos != newCursorPos)
			ViewPort::GetInstance().RecalculateMouseCoords();
	}
	glm::vec3 rot = Window::GetCamera().GetRotation();
	if (rot.y > glm::two_pi<float>() || rot.y < -glm::two_pi<float>())
		rot.y = 0;
	if (rot.x > glm::two_pi<float>() || rot.x < -glm::two_pi<float>())
		rot.x = 0;
	Window::GetCamera().SetRotation(glm::vec3(rot.x, rot.y, 0.0f));

}

void Doom::Window::SetCursorPos(glm::dvec2 pos)
{
	glfwSetCursorPos(Window::GetWindow(), pos.x, pos.y);
}