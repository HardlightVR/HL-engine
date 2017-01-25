#pragma once
#include <unordered_map>
#include "Atom.h"
#include "boost\uuid\uuid.hpp"
class HapticsExecutor;
class PriorityModel;
class IPlayable {
public:
	virtual ~IPlayable() = default;
	virtual void Play() = 0;
	virtual void PlayFromStart() = 0;
	virtual void Reset() = 0;
	virtual void Pause() = 0;
	virtual void Update(float dt, const std::unordered_map<std::string, Atom>&) = 0;
	virtual uint32_t GetHandle() const = 0;
	virtual float GetTotalPlayTime() const = 0;
	virtual float CurrentTime() const = 0;
	virtual bool IsPlaying() const = 0;
	virtual void Release() const = 0;
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



