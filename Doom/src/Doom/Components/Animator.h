#pragma once

namespace Doom {

	class DOOM_API Animator : public Component {

		std::vector<std::string> texturesPath;
		double timer = 0;
		const char** items = nullptr;
		int counter = 0;
		int amount = 0;
		int selectedanim = 0;

		friend class Editor;

	public:

		std::map<std::string, std::vector<Texture*>> animations;
		const char** GetAnimations();
		bool isPlayingAnim = false;
		float speed = 8;

		Animator();
		~Animator() {
			delete items;
		}

		void SetAnimation(const std::string path);
		void PlayAnim(std::vector<Texture*>& text);
		int GetAmountOfAnimations();
	};

}