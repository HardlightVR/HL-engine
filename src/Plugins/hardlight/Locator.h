#pragma once
#include "ILogger.h"
#include "NullLogger.h"
#include "EnumTranslator.h"
#include <iostream>

class Locator
{
public:
	static void initialize() { }

	static EnumTranslator& Translator() { return *_translator; }
	

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

};
