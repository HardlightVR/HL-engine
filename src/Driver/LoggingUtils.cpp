#include "stdafx.h"
#include "LoggingUtils.h"
std::string LevelString(LogLevel level) {

	if (level == LogLevel::Info) {
		return "Info";
	}
	else if (level == LogLevel::Warning) {
		return "Warning";
	}
	else if (level == LogLevel::Error) {
		return "Error";
	}
	else if (level == LogLevel::Error) {
		return "Fatal";
	}
	else {
		return "";
	}
}