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
	PlayableSequence(std::vector<JsonSequenceAtom>, AreaFlag loc);
	~PlayableSequence();
	void Play() override;
	void Reset(PriorityModel & model) override;
	void Pause(PriorityModel & model) override;
	void Update(float dt, PriorityModel & model) override;
	uint32_t GetHandle() const override;
private:
	std::vector<MyInstant> _effects;
	std::vector<JsonSequenceAtom> _sourceOfTruth;
	std::vector<boost::uuids::uuid> _activeEffects;
	bool _paused;
	int _handle;
	AreaFlag _location;
};


