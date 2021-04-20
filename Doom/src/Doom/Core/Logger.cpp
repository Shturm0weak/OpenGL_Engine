#include "pch.h"
#include "Logger.h"

void Doom::Logger::UpdateTime()
{
	std::time_t time = std::time(0);
	Logger::s_CurrentTime = std::localtime(&time);
	Logger::s_TimeString = "["
		+ std::to_string(Logger::s_CurrentTime->tm_hour)
		+ ":" + std::to_string(Logger::s_CurrentTime->tm_min)
		+ ":" + std::to_string(Logger::s_CurrentTime->tm_sec)
		+ "]";
}

void Doom::Logger::Log(const char* message, const char* color, const char* type)
{
	std::cout << color << Logger::s_TimeString << type << ":" << message << RESET << "\n";
}

void Doom::Logger::Log(const char* message, const char* withWhat, const char* where, const char* color, const char* type)
{
	std::cout << color << Logger::s_TimeString << type << ":" << NAMECOLOR << withWhat << color << ": <" << NAMECOLOR << where << color << "> " << message << "\n" << RESET;
}

void Doom::Logger::Error(const char* message)
{
	Logger::Log(message, BOLDRED, "ERROR");
}

void Doom::Logger::Error(const char* message, const char* withWhat, const char* where)
{
	Log(message, withWhat, where, BOLDRED, "ERROR");
}

void Doom::Logger::Warning(const char* message)
{
	Logger::Log(message, BOLDYELLOW, "WARNING");
}

void Doom::Logger::Warning(const char* message, const char* withWhat, const char* where)
{
	Log(message, withWhat, where, BOLDYELLOW, "WARNING");
}

void Doom::Logger::Success(const char* message)
{
	Logger::Log(message, BOLDGREEN, "SUCCESS");
}

void Doom::Logger::Success(const char* message, const char* withWhat, const char* where)
{
	Log(message, withWhat, where, BOLDGREEN, "SUCCESS");
}
