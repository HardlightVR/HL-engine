#pragma once
#include "EnumTranslator.h"
class Locator
{
public:
	static void initialize();
	static EnumTranslator& getTranslator() { return _translator; }

private:
	static EnumTranslator _translator;

};

