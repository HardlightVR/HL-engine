#pragma once
#include "StdAfx.h"
#include "Enums.h"
#include <unordered_map>
class EnumTranslator
{
public:
	EnumTranslator();
	~EnumTranslator();
	const char* ToString(Location loc);
	const char* ToString(Effect effect);
private:
	std::unordered_map<Location, const char*> locations;
	std::unordered_map<Effect, const char*> effects;
	void init_locations();
	void init_effects();
};

