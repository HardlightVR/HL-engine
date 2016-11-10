#pragma once
#include "IPlayable.h"
#include "TimeInstant.h"

#define ERROR_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE(...) __VA_ARGS__>::type
#define FUNCTION_REQUIRES(...) typename boost::enable_if<boost::mpl::and_<__VA_ARGS__, boost::mpl::bool_<true> >, ERROR_PARENTHESIS_MUST_BE_PLACED_AROUND_THE_RETURN_TYPE
#define EXCLUDE(...) typename boost::mpl::not_<typename boost::mpl::or_<__VA_ARGS__, boost::mpl::bool_<false> >::type >::type

struct SequenceComparator
{
	bool operator()(JsonSequenceAtom atom, float atomTime)
	{
		return atomTime >= atom.Time;
	}
};

template<typename T>
class Instant {
public:
	T Item;
	float Time;
	bool Executed;
	boost::uuids::uuid Handle;
	
	std::function<bool(T i, float rhs)> comp;
	Instant(float t, T i, std::function<bool(T i, float rhs)> c) :Time(t), Item(i), Executed(false), comp(c) {}
	bool Expired() {
		return comp(Item, Time);
	}
};



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
	std::function<bool(JsonSequenceAtom a, float)> _comparator; 

	std::vector<Instant<JsonSequenceAtom>> _effects;
	std::vector<JsonSequenceAtom> _sourceOfTruth;
	std::vector<boost::uuids::uuid> _activeEffects;
	bool _paused;
	int _handle;
	AreaFlag _location;
};


