#pragma once
#include "IPlayable.h"
#include "TimeInstant.h"


class MyInstant {
public:
	JsonSequenceAtom Item;
	float Time;
	MyInstant(float t, JsonSequenceAtom i) :Time(t), Item(i) {};
	bool Expired() const {
		return Time > Item.Time;
	}
};
class PlayableSequence : public IPlayable {
public:
	PlayableSequence(std::vector<JsonSequenceAtom>, Area loc);
	~PlayableSequence();
	void Play() override;
	void Pause() override;
	void Update(float dt, std::unordered_map<Location, HapticQueue>& model) override;
	int GetHandle() const override;
private:
	std::vector<JsonSequenceAtom> _effects;
	std::vector<MyInstant> _liveEffects;
	bool _paused;
	int _handle;
	Area _location;
};


