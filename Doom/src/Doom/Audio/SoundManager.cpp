#include "SoundManager.h"

using namespace Doom;

void SoundManager::CreateSoundAsset(std::string name,Sound * sound)
{
	alGenBuffers(1, &sound->m_Buffer);
	alBufferData(sound->m_Buffer, AL_FORMAT_STEREO16, sound->soundBuffer, sound->numberOfSamples * sound->m_Channels * 2, sound->m_SampleRate);
	alGenSources(1, &sound->m_Source);
	alSourcei(sound->m_Source, AL_BUFFER, sound->m_Buffer);
	s_Sounds.insert(std::make_pair(name,sound));
	sound->m_Volume = 1;
	sound->m_State = STOPPED;
}

void SoundManager::Init()
{
	s_AlcDevice = alcOpenDevice(NULL);
	s_AlcContext = alcCreateContext(s_AlcDevice, NULL);
	alcMakeContextCurrent(s_AlcContext);
	std::cout << BOLDGREEN << "Initialized SoundManager\n" << RESET;
}

void SoundManager::Resume(Sound * sound)
{
	alSourcePlay(sound->GetId());
}

void Doom::SoundManager::Play(Sound * sound)
{
	alSourceStop(sound->GetId());
	Resume(sound);
}

void SoundManager::Loop(Sound * sound)
{
	alSourceStop(sound->GetId());
	alSourcei(sound->GetId(), AL_LOOPING, AL_TRUE);
	alSourcePlay(sound->GetId());
}

void Doom::SoundManager::Pause(Sound * sound)
{
	alSourcePause(sound->GetId());
}

void Doom::SoundManager::Stop(Sound * sound)
{
	alSourceStop(sound->GetId());
}

Sound* SoundManager::GetSound(std::string name)
{
	auto sound = s_Sounds.find(name);
	if (sound != s_Sounds.end())
		return sound->second;
	return nullptr;
}

void Doom::SoundManager::SetVolume(float volume)
{
	alListenerf(AL_GAIN, volume);
}

void Doom::SoundManager::UpdateSourceState()
{
	for (auto i = s_Sounds.begin(); i != s_Sounds.end(); i++)
	{
		ALint state;
		alGetSourcei(i->second->GetId(), AL_SOURCE_STATE, &state);

		switch (state)
		{
		case AL_PAUSED:
			i->second->m_State = PAUSED;
			break;
		case AL_PLAYING:
			i->second->m_State = PLAYING;
			break;
		case AL_INITIAL:
		case AL_STOPPED:
			i->second->m_State = STOPPED;
			break;
		default:
			i->second->m_State = STOPPED;
			break;
		}
	}
}

void SoundManager::ShutDown()
{
	for (auto i = s_Sounds.begin(); i != s_Sounds.end(); i++)
	{
		delete i->second;
	}
	s_Sounds.clear();
}
