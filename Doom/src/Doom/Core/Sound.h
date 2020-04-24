#pragma once
#include "Core.h"
#include <Windows.h>
#include <mmsystem.h>
#include "ThreadPool.h"

namespace Doom {

	class DOOM_API Sound {
		static bool Playimplementation(const wchar_t* filepath, unsigned int volume = 65535);
	public:
		static void Play(const wchar_t* filepath, unsigned int volume = 65535);
	};

}