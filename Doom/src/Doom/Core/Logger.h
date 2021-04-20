#pragma once

namespace Doom {

	class DOOM_API Logger {
	private:

		static std::tm* s_CurrentTime;
		static std::string s_TimeString;

		static void UpdateTime();

		friend class EntryPoint;
	public:

		static void Log(const char* message, const char* color = RESET, const char* type = "LOG");
		static void Log(const char* message, const char* withWhat, const char* where, const char* color, const char* type = "LOG");
		static void Error(const char* message);
		static void Error(const char* message, const char* withWhat, const char* where);
		static void Warning(const char* message);
		static void Warning(const char* message, const char* withWhat, const char* where);
		static void Success(const char* message);
		static void Success(const char* message, const char* withWhat, const char* where);
	};

}