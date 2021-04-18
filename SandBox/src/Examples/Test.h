#pragma once

class Test : public Application {
public:

	GameObject* m_HalfCircle = nullptr;
	double m_RotationSpeed = 360.0;
	double m_TotalRad = 0.0;
	double m_CurrentRound = 0.0;
	int64_t m_Rounds = 0;
	bool m_HasBeenStarted = false;

	Test(std::string name = "Test", float x = 800, float y = 600, bool Vsync = false) : Application(name, TYPE_2D, x, y, Vsync) {}

	void OnStart()
	{
		new Line(glm::vec3(-100000,0,0), glm::vec3(100000,0,0));
		m_HalfCircle = GameObject::Create("HalfCircle");
		m_HalfCircle->AddComponent<SpriteRenderer>()->m_Texture = Texture::Create("src/Images/HalfCircle.png");
		m_HalfCircle->GetComponent<SpriteRenderer>()->m_Color = COLORS::Red;
	}

	void OnUpdate()
	{
		if (m_HasBeenStarted)
		{
			double speed = glm::radians(-DeltaTime::s_Deltatime * m_RotationSpeed);
			m_TotalRad = m_HalfCircle->m_Transform.GetRotation().z;
			m_CurrentRound += speed;
			if (abs(m_CurrentRound) > glm::two_pi<double>())
			{
				m_CurrentRound = 0.0;
				m_Rounds -= 1;
			}
			if (m_Rounds == 0)
			{
				m_HalfCircle->m_Transform.RotateOnce(glm::vec3(0.0f, 0.0f, 0.0f), true);
				m_HasBeenStarted = false;
				m_TotalRad = 0.0;
			}
			else
			{
				m_HalfCircle->m_Transform.Rotate(glm::vec3(0.0f, 0.0f, m_TotalRad + speed), true);
			}
		}
	}

	void OnGuiRender()
	{
		Gui& g = Gui::GetInstance();
		float aspectRatio = Window::GetInstance().GetCamera().GetAspectRatio();
		if (!m_HasBeenStarted)
		{
			g.RelateToPanel();
			g.m_RelatedPanelProperties.m_AutoAllignment = true;
			g.Panel(L"##1", -((float)1080 * 0.8f), ((float)1080 * (1.0 / aspectRatio) * 0.6f), 400, 400, COLORS::Gray * 0.3f);
			g.InputInt(L"Rounds", &m_Rounds, 0, 0);
			g.UnRelateToPanel();

			if (g.Button(L"Start") && m_Rounds > 0)
			{
				m_HasBeenStarted = true;
			}
		}
		else
		{
			g.RelateToPanel();
			g.m_RelatedPanelProperties.m_AutoAllignment = true;
			g.Panel(L"##2", ((float)1080 * 0.8f), ((float)1080 * (1.0 / aspectRatio) * 0.6f), 400, 400, COLORS::Gray * 0.3f);
			g.Text(L"Rounds %d", true, 0.0f, 0.0f, 20.0f, COLORS::White, 0, m_Rounds);
			g.UnRelateToPanel();
		}
	}

};