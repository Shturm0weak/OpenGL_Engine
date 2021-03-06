#pragma once 

namespace Doom {

	class StressTest2D : public Doom::Application {
	public:
		virtual void OnStart() override {
			for (size_t i = 0; i < 316; i++)
			{
				for (size_t j = 0; j < 316; j++)
				{
					GameObject* go = GameObject::Create(std::to_string(i) + " " + std::to_string(j), i, j);
					go->m_ComponentManager.AddComponent<SpriteRenderer>();
					go->m_Transform->Scale(glm::vec3(0.9));
				}
			}
		}

		virtual void OnGuiRender() override {
			Gui::GetInstance().Text("%f", true, 0, 0, 40, COLORS::Red, 0, Window::GetInstance().GetFPS());
		}
	};

}