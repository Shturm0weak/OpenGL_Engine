#include "Sound.h"

bool Doom::Sound::Playimplementation(const wchar_t * filepath, unsigned int volume)
{
	waveOutSetVolume(0, MAKELONG(volume, volume));
	return PlaySound(filepath, NULL, SND_SYNC);
}
void Doom::Sound::Play(const wchar_t * filepath, unsigned int volume)
{
	auto f = std::bind(&Sound::Playimplementation, filepath, volume);
	ThreadPool::Instance()->enqueue(f);
}
