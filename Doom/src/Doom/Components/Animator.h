#pragma once

#include "../Core/Core.h"
#include "../OpenGL/Texture.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API Animator : public Component {
	private:

		std::vector<std::string> m_TexturesPath;
		double m_Timer = 0;
		const char** items = nullptr;
		int m_Counter = 0;
		int m_Amount = 0;
		int m_SelectedAnim = 0;

		friend class Editor;
	public:

		std::map<std::string, std::vector<Texture*>> animations;
		const char** GetAnimations();
		bool m_IsPlayingAnim = false;
		float m_Speed = 8;

		Animator();
		~Animator() {
			delete items;
		}

		void SetAnimation(const std::string path);
		void PlayAnim(std::vector<Texture*>& text);
		inline int GetAmountOfAnimations() const { return m_Amount; }
	};

}