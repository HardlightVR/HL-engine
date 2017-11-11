#pragma once

#include "Enums.h"
#include <mutex>
#include <boost/optional/optional.hpp>
#include "HardwareCommands.h"
#include "BufferedEvent.h"

#include <boost/lockfree/spsc_queue.hpp>
class RtpModel {
public:
	
	RtpModel(Location area);
	void Put(BufferedEvent event);
	void ChangeVolume(int newVolume);
	int GetVolume();
	CommandBuffer Update(float dt);
private:
	enum class State {
		PlayingSomething,
		NotPlayingSomething
	};
	boost::lockfree::spsc_queue<double> m_samples;
	int volume;
	Location location;
	double lastSampled;
	boost::optional<PlayVol> volumeCommand;
	std::mutex volumeValueProtector;
	State m_state;
};