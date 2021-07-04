#pragma once

class Test : public Application {
public:

	Test(std::string name = "Test", float x = 800, float y = 600, bool Vsync = false) : Application(name, x, y, Vsync) {}

	float value = 0.0f;
	bool checkBox = false;
	int64_t integer = 0;

	void OnStart()
	{
		setlocale(LC_ALL, "all");
	}

	void OnUpdate()
	{

	}

	void OnGuiRender()
	{
		Gui& g = Gui::GetInstance();
		ViewPort& viewport = ViewPort::GetInstance();
		glm::vec2 size = viewport.GetSize() * 3.0f;
		float ratio = viewport.GetAspectRatio();
		float textSize = std::fmin(size.x / size.y, size.y / size.x);
		g.m_RelatedPanelProperties.m_Margin = glm::vec2(0);
		g.m_RelatedPanelProperties.m_Padding = glm::vec2(0.0f, size.y * 0.01);
		g.RelateToPanel();
		g.m_RelatedPanelProperties.m_AutoAllignment = true;
		g.Panel(L"##Properties", 0, 0, 0.3 * size.x, 0.3 * size.y, glm::vec4(0.3, 0.3, 0.3, 0.8));
		g.Text(L"Текст", true, 0.0f, 0.0f, g.m_Font->m_Size * textSize * ratio);
		if (g.Button(L"Выход", 0, 0, g.m_Font->m_Size * textSize * ratio, 0.05 * size.x, 0.05 * size.y))
		{
			Window::GetInstance().Exit();
		}
		g.SliderFloat(L"Ползунок", &value, 0.0f, 1.0, 0.0f, 0.0f, 0.05 * size.x, 0.025 * size.y);
		g.Bar(0, 0, value, 1, COLORS::Green, COLORS::Blue, 0.05 * size.x, 0.025 * size.y);
		g.CheckBox(L"Флажок", &checkBox, 0.0f, 0.0f, g.m_Font->m_Size * textSize * ratio);
		g.CollapsingHeader(L"Collapsing header", 0.0, 0.0, 0.025 * size.y, COLORS::Gray);
		g.InputInt(L"Целое число", &integer, 0.0f, 0.0f, 0.05 * size.x, 0.025 * size.y);
		g.m_RelatedPanelProperties.m_AutoAllignment = false;
		g.UnRelateToPanel();
		//std::cout << ViewPort::GetInstance().GetStaticMousePosition().x << " : " << ViewPort::GetInstance().GetStaticMousePosition().y << std::endl;
	}

	void OnClose()
	{

	}

};