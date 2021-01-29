#pragma once
#include "Sound.h"

namespace Doom {

	class DOOM_API SoundManager {
	private:

		ALCdevice* s_AlcDevice = nullptr;
		ALCcontext* s_AlcContext = nullptr;
		
		SoundManager() {}
		SoundManager(const SoundManager&) = delete;
		SoundManager& operator=(const SoundManager&) { return *this; }
	public:

		std::map<std::string, Sound*> s_Sounds;

		static SoundManager& GetInstance();
		Sound* GetSound(std::string name);
		void CreateSoundAsset(std::string name,Sound* sound);
		void Init();
		void Resume(Sound* sound);
		void Play(Sound* sound);
		void Loop(Sound* sound);
		void Pause(Sound* sound);
		void Stop(Sound* sound);
		void SetVolume(float volume);
		void UpdateSourceState();
		void ShutDown();
	};

}