#pragma once

#include "Application.h"

namespace Doom {

	class AimTrainer : public Application {
	private:

		Texture* m_CrossHair = nullptr;
		std::vector<std::string> m_IgnoreMask;
		std::map<GameObject*, double> m_Objs; //time since spawn
		glm::vec2 m_LimX{ -5.0f, 5.0f };
		glm::vec2 m_LimY{ 5.0f, 10.0f };
		glm::vec2 m_LimZ{ -5.0f, 5.0f };
		uint32_t m_Missed = 0;
		uint32_t m_Hits = 0;
		double m_Time = 0;
		float m_CrossHairScale = 1.0f;
		float m_AudioVolume = 0.5f;
		float m_MouseSensetivity = 1.0f;
		bool m_Pause = true;
		bool m_Options = false;
		bool m_ChooseCrossHair = false;
		bool m_DrawShadows = true;
		void OptionsMenu();
		void ChooseCrossHairMenu();
		void MainMenu();
		void RayCast();
		void CameraMovement();
		void SpawnObject();
		void UpdateObjects();
		glm::vec3 RandomPos(glm::vec2 limX, glm::vec2 limY, glm::vec2 limZ);

	public:

		AimTrainer(std::string name = "AimTrainer", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}

		void OnStart();
		void OnUpdate();
		void OnClose();
		void OnGuiRender();
	};

}