#include "StdAfx.h"
#include "Atom.h"
#include "json.h"
#include <cassert>
#include "Locator.h"
Atom::Atom()
{
}


Atom::~Atom()
{
}

std::string Atom::Id() const
{
	return _id;
}

Effect Atom::GetEffect(float volume) const
{
	//clamp between 0.0 and 1.0
	volume = std::min(1.0f, std::max(volume, 0.0f));
	int index = round((_numLevels-1) * volume);
	assert(index >= 0);
	assert(index <= _numLevels - 1);
	return _levels.at(index);

}

void Atom::Serialize(const Json::Value & root)
{
}

void Atom::Deserialize(const Json::Value & root)
{
	_id = root.get("id", "error").asString();
	
	auto& t = Locator::Translator();
	for (const Json::Value& level : root["levels"]) {
		_levels.push_back(t.ToEffect(level.asString()));
	}
	_numLevels = _levels.size();
}
