#pragma once
#include "ILogger.h"


class NullLogger : public ILogger
{
public:
	void Log(std::string tag, std::string message) override {}
	void Log(std::string message) override {}
	void Log(std::string tag, std::string message, LogLevel level) override {}

	NullLogger() {}
	~NullLogger() {}
};



