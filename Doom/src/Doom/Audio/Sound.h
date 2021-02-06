#pragma once

#include "../Core/Core.h"
#include "../Enums/ColoredOutput.h"

#include <string>
#include <iostream>
#include <map>

#include "OpenAl/OpenAl/al.h"
#include "OpenAl/OpenAl/alc.h"

namespace Doom {

	enum AudioState {
		PLAYING,
		STOPPED,
		PAUSED
	};

	class DOOM_API Sound {
	private:

		std::string m_FileName;
		ALuint m_Buffer;
		ALuint m_Source;
		AudioState m_State;
		int m_Channels;
		int m_SampleRate;
		short* m_SoundBuffer;
		int m_NumberOfSamples;
		float m_Volume;
		bool m_IsLooped;

		friend class SoundManager;
	public:
		
		Sound(std::string filename);
		~Sound();

		void SetVolume(float volume);
		int GetState() { return (int)m_State; }
		ALuint GetId() { return m_Source; }
	};

}