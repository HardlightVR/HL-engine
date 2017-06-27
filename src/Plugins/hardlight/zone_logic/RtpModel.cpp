#include "stdafx.h"
#include "RtpModel.h"



RtpModel::RtpModel(Location area) : 
	location(area), 
	volume(0), 
	volumeValueProtector(),
	volumeCommand(boost::none)
{
}

void RtpModel::ChangeVolume(int newVolume)
{
	

	std::lock_guard<std::mutex> guard(volumeValueProtector);

	if (newVolume != volume) {
		volume = newVolume;
	
		volumeCommand = PlayVol(location, volume / 2);
	}
	else {
		volumeCommand = boost::none;
	}

}

CommandBuffer RtpModel::Update(float dt)
{
	CommandBuffer commands;

	std::lock_guard<std::mutex> guard(volumeValueProtector);

	if (volumeCommand) {
		commands.push_back(std::move(*volumeCommand));
		volumeCommand = boost::none;
	}

	return commands;
}

int RtpModel::GetVolume()
{
	return volume;
}