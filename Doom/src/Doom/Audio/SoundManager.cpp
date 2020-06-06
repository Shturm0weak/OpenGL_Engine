#include "SoundManager.h"

using namespace Doom;

void SoundManager::CreateSoundAsset(std::string name,Sound * sound)
{
	alGenBuffers(1, &sound->buffer);
	alBufferData(sound->buffer, AL_FORMAT_STEREO16, sound->soundBuffer, sound->numberOfSamples * sound->channels * 2, sound->sampleRate);
	alGenSources(1, &sound->source);
	alSourcei(sound->source, AL_BUFFER, sound->buffer);
	sounds.insert(std::make_pair(name,sound));
	sound->volume = 1;
	sound->state = STOPPED;
}

void SoundManager::Init()
{
	alcDevice = alcOpenDevice(NULL);
	alcContext = alcCreateContext(alcDevice, NULL);
	alcMakeContextCurrent(alcContext);
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
	auto sound = sounds.find(name);
	if (sound != sounds.end())
		return sound->second;
	return nullptr;
}

void Doom::SoundManager::UpdateSourceState()
{
	for (auto i = sounds.begin(); i != sounds.end(); i++)
	{
		ALint state;
		alGetSourcei(i->second->GetId(), AL_SOURCE_STATE, &state);

		switch (state)
		{
		case AL_PAUSED:
			i->second->state = PAUSED;
			break;
		case AL_PLAYING:
			i->second->state = PLAYING;
			break;
		case AL_INITIAL:
		case AL_STOPPED:
			i->second->state = STOPPED;
			break;
		default:
			i->second->state = STOPPED;
			break;
		}
	}
}

void SoundManager::ShutDown()
{
	for (auto i = sounds.begin(); i != sounds.end(); i++)
	{
		delete i->second;
	}
	sounds.clear();
}
