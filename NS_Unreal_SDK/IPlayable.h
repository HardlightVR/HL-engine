#pragma once
#include <unordered_map>
#include "Atom.h"
class PriorityModel;
class IPlayable {
public:
	virtual ~IPlayable() = default;
	virtual void Play() = 0;
	virtual void Reset(PriorityModel& model) = 0;
	virtual void Pause(PriorityModel& model) = 0;
	//virtual void Resume() = 0;
	//virtual void Cancel() = 0;
	virtual void Update(float dt, PriorityModel& model, const std::unordered_map<std::string, Atom>&) = 0;
	virtual uint32_t GetHandle() const = 0;
};


template<typename T>
class Instant {
public:
	T Item;
	float ItemTime;
	float Time;
	bool Executed;
	boost::uuids::uuid Handle;

	Instant(T item, float itemTime) :Item(item), Time(0.0), ItemTime(itemTime), Executed(false) {}

	bool Expired() {
		return Time >= ItemTime;
	}
};

