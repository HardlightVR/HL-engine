 #pragma once
#include <string>


enum class LogLevel {
	Info,
	Warning,
	Error,
	Fatal
};

class ILogger
{
public:
	virtual ~ILogger() {}
	virtual void Log(std::string tag, std::string message) = 0;
	virtual void Log(std::string tag, std::string message, LogLevel level) = 0;
	virtual void Log(std::string message) = 0;

};



