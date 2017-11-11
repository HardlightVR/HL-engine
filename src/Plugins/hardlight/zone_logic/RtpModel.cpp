#include "stdafx.h"
#include "RtpModel.h"


RtpModel::RtpModel(Location area) : 
	location(area), 
	volume(0), 
	volumeValueProtector(),
	m_samples(2048),
	volumeCommand(boost::none),
	m_state(State::NotPlayingSomething)
{
}

void RtpModel::Put(BufferedEvent event)
{
	//naive at first. Just putting all the samples in the queue. No layering, etc.
	auto data = event.Data();
	
	for (double d : data) {
		m_samples.push(d);
	}
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
	State oldState = m_state;

	if (!m_samples.empty()) {
		m_state = State::PlayingSomething;
		double vol = m_samples.front();
		m_samples.pop();
		double val = vol * 255;
		assert(val <= 255);
		commands.push_back(PlayVol(location, static_cast<uint16_t>(val)/ 2));
	}
	else {
		m_state = State::NotPlayingSomething;
	}


	if (oldState == State::PlayingSomething && m_state == State::NotPlayingSomething) {
		commands.push_back(PlayVol(location, 0));
	}
	

/*
	if (volumeCommand) {
		commands.push_back(std::move(*volumeCommand));
		volumeCommand = boost::none;
	}
*/
	return commands;
}

int RtpModel::GetVolume()
{
	return volume;
}