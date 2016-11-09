#pragma once
#include "IPlayable.h"
#include "TimeInstant.h"


class MyInstant {
public:
	JsonSequenceAtom Item;
	float Time;
	bool Executed;
	boost::uuids::uuid Handle;
	MyInstant(float t, JsonSequenceAtom i) :Time(t), Item(i), Executed(false) {};
	bool Expired() const {
		return Time > Item.Time;
	}
};
class PlayableSequence : public IPlayable {
public:
	PlayableSequence(std::vector<JsonSequenceAtom>, Area loc);
	~PlayableSequence();
	void Play() override;
	void Reset() override;
	void Pause(std::unordered_map<Location, HapticQueue> & model) override;
	void Update(float dt, std::unordered_map<Location, HapticQueue>& model) override;
	int GetHandle() const override;
private:
	std::vector<MyInstant> _effects;
	std::vector<JsonSequenceAtom> _sourceOfTruth;
	std::vector<boost::uuids::uuid> _activeEffects;
	bool _paused;
	int _handle;
	Area _location;
};


