#pragma once

#include "Enums.h"
#include <mutex>
#include <boost/optional/optional.hpp>
#include "HardwareCommands.h"
class RtpModel {
public:
	RtpModel(Location area);
	void ChangeVolume(int newVolume);
	int GetVolume();
	CommandBuffer Update(float dt);
private:
	int volume;
	Location location;

	boost::optional<PlayVol> volumeCommand;
	std::mutex volumeValueProtector;
};