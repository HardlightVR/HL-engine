#pragma once
#include "ILogger.h"
#include "NullLogger.h"
#include "EnumTranslator.h"

class Locator
{
public:
	static void initialize() { _logger = &_nullLogger; }

	static ILogger& Logger() { return *_logger; }
	static EnumTranslator& Translator() { return *_translator; }
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

	static void provide(EnumTranslator* service) {
		if (service == nullptr) {
			std::cout << "Don't provide a null translator okay?\n";
		}
		else {
			_translator = service;
		}
	}

private:
	static EnumTranslator* _translator;

	static ILogger* _logger;
	static NullLogger _nullLogger;
};
