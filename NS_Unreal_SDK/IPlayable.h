#pragma once
#include "HapticsExecutor.h"
class IPlayable {
public:
	virtual ~IPlayable() = default;
	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;
	virtual void Cancel() = 0;
	virtual void Update(float dt, Model& model) = 0;
	virtual int GetHandle() const = 0;
};