#include "Sound.h"
#include "wavParser.h"
#include "vorbis.h"

using namespace Doom;

void Doom::Sound::SetVolume(float volume)
{
	alSourcef(source, AL_GAIN, volume);
}

Sound::Sound(std::string filename) : m_fileName(filename){
	std::string format = m_fileName.substr(m_fileName.find_last_of(".") + 1);
	if (format == "ogg") {
		numberOfSamples = stb_vorbis_decode_filename(m_fileName.c_str(), &channels, &sampleRate, &soundBuffer);
		if (numberOfSamples < 0) {
			std::cout << RED << "fail to Load\n" << RESET;
			return;
		}
	}
	else if (format == "wav") {
		numberOfSamples = WavReader(m_fileName.c_str(), &channels, &sampleRate, &soundBuffer);
		if (numberOfSamples < 0) {
			std::cout << RED << "fail to Load\n" << RESET;
			return;
		}
	}

	
}

Sound::~Sound() {
	delete soundBuffer;
}