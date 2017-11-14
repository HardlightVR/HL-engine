#include "StdAfx.h"
#include "Atom.h"
#include "Json/json.h"
#include <cassert>
#include "Locator.h"

std::string Atom::Id() const
{
	return _id;
}

Effect Atom::GetEffect(float volume) const
{
	//clamp between 0.0 and 1.0
	volume = std::min(1.0f, std::max(volume, 0.0f));
	assert(_numLevels > 0);
	std::size_t index = (int) round((_numLevels-1) * volume);
	
	return _levels.at(index);

}

int Atom::GetDuration() const
{
	return _duration;
}

void Atom::Serialize(const Json::Value & root)
{
}

void Atom::Deserialize(const Json::Value & root)
{
	_id = root.get("id", "error").asString();
	_duration = root.get("duration", 150).asUInt();
	auto& t = Locator::Translator();
	for (const Json::Value& level : root["levels"]) {
		_levels.push_back(t.ToEffect(level.asString()));
	}
	_numLevels = _levels.size();
}
