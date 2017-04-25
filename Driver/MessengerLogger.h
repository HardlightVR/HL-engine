#pragma once
#include "ILogger.h"
#include "LoggingUtils.h"
#include <iostream>
#include <iomanip>

#include "DriverMessenger.h"
class MessengerLogger : public ILogger
{
public:
	typedef std::function<void(std::string)> StringWriter;
	void Log(std::string tag, std::string message) {

	}
	void Log(std::string tag, std::string message, LogLevel level) {

	}
	void Log(std::string message) {
		m_stream(message);
	}
	MessengerLogger(StringWriter m) :m_stream(m) {}
	~MessengerLogger();
private:
	StringWriter m_stream;
};

