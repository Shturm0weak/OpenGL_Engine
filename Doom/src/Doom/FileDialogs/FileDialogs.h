#pragma once

#include <optional>

namespace Doom {

	class DOOM_API FileDialogs
	{
	public:
		static std::optional<std::string> OpenFile(const char* filter);
		static std::optional<std::string> SaveFile(const char* filter);
	};

}

