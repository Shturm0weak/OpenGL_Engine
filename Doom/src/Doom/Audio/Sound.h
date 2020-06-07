#pragma once

#include "../Core/Core.h"
#include "../Core/ColoredOutput.h"

#include <string>
#include <iostream>
#include <map>

#include "OpenAl/OpenAl/al.h"
#include "OpenAl/OpenAl/alc.h"

#define STB_VORBIS_HEADER_ONLY
#include "vorbis.h"
#undef STB_VORBIS_HEADER_ONLY

namespace Doom {

	enum AudioState {
		PLAYING,
		STOPPED,
		PAUSED
	};

	class DOOM_API Sound {
	private:
		std::string m_fileName;
		ALuint buffer;
		ALuint source;
		int channels;
		int sampleRate;
		short* soundBuffer;
		int numberOfSamples;
		AudioState state;
		float volume;
		bool loop;

		friend class SoundManager;
	public:
		
		void SetVolume(float volume);
		Sound(std::string filename);
		~Sound();

		inline int GetState() { return state; }
		inline ALuint GetId() { return source; }
	};

}