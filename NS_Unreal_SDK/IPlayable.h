#pragma once
#include <unordered_map>
class PriorityModel;
class IPlayable {
public:
	virtual ~IPlayable() = default;
	virtual void Play() = 0;
	virtual void Reset(PriorityModel& model) = 0;
	virtual void Pause(PriorityModel& model) = 0;
	//virtual void Resume() = 0;
	//virtual void Cancel() = 0;
	virtual void Update(float dt, PriorityModel& model) = 0;
	virtual uint32_t GetHandle() const = 0;
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
