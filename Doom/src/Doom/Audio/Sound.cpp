#include "Sound.h"
#include "wavParser.h"
#include "vorbis.h"

using namespace Doom;

void Doom::Sound::SetVolume(float volume)
{
	alSourcef(m_Source, AL_GAIN, volume);
}

Sound::Sound(std::string filename) : m_FileName(filename)
{
	std::string format = m_FileName.substr(m_FileName.find_last_of(".") + 1);
	if (format == "ogg") 
	{
		m_NumberOfSamples = stb_vorbis_decode_filename(m_FileName.c_str(), &m_Channels, &m_SampleRate, &m_SoundBuffer);
		if (m_NumberOfSamples < 0)
		{
			std::cout << RED << "fail to Load\n" << RESET;
			return;
		}
	}
	else if (format == "wav") 
	{
		m_NumberOfSamples = WavReader(m_FileName.c_str(), &m_Channels, &m_SampleRate, &m_SoundBuffer);
		if (m_NumberOfSamples < 0) 
		{
			std::cout << RED << "fail to Load\n" << RESET;
			return;
		}
	}

	
}

Sound::~Sound()
{
	delete m_SoundBuffer;
}