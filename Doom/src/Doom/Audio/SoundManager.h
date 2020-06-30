#pragma once
#include "Sound.h"

namespace Doom {

	class DOOM_API SoundManager {
	private:
		static ALCdevice* alcDevice;
		static ALCcontext* alcContext;
	public:
		static std::map<std::string, Sound*> sounds;
		static Sound* GetSound(std::string name);
		static void CreateSoundAsset(std::string name,Sound* sound);
		static void Init();
		static void Resume(Sound* sound);
		static void Play(Sound* sound);
		static void Loop(Sound* sound);
		static void Pause(Sound* sound);
		static void Stop(Sound* sound);
		static void SetVolume(float volume);
		static void UpdateSourceState();
		static void ShutDown();
	};

}