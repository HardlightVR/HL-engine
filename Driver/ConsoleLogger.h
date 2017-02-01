#pragma once
#include "ILogger.h"
#include "LoggingUtils.h"
#include <iostream>
#include <iomanip>
class ConsoleLogger : public ILogger
{
public:
	void Log(std::string tag, std::string message) override { 
		Log(tag, message, LogLevel::Info);
	}
	void Log(std::string message) override { 
		std::cout << std::setw(10) << getTimestamp() << message << '\n'; 
	}
	void Log(std::string tag, std::string message, LogLevel level) override {
		std::cout << 
			std::setw(10) << std::left << getTimestamp() << 
			std::setw(8)  << std::left << formatLevel(LevelString(level)) << 
			std::setw(14) << formatTag(tag) << message << '\n';
	}


	ConsoleLogger();
	~ConsoleLogger();
private:

	std::string formatTag(const std::string& tag) const {
		return "[" + tag + "]";
	}
	std::string formatLevel(const std::string& level) const {
		return "(" + level + ")";
	}

	std::string getTimestamp() {
		auto t = std::time(nullptr);
		struct tm buf;
		localtime_s(&buf, &t);
		std::ostringstream oss;
		oss << std::put_time(&buf, "%H:%M:%S");
		return oss.str();
	}
};

