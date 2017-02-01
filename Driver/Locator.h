#pragma once
#include "ILogger.h"
#include "NullLogger.h"


class Locator
{
public:
	static void initialize() { _logger = &_nullLogger; }

	static ILogger& Logger() { return *_logger; }

	static void provide(ILogger* service)
	{
		if (service == nullptr)
		{
			// Revert to null service.
			_logger = &_nullLogger;
		}
		else
		{
			_logger = service;
		}
	}

private:
	static ILogger* _logger;
	static NullLogger _nullLogger;
};
