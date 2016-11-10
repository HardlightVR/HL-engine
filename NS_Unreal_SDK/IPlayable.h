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