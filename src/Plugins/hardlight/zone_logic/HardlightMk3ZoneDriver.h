#pragma once

#include "Enums.h"

#include "ZoneModel.h"
#include "RtpModel.h"
#include <boost/uuid/random_generator.hpp>
#include "HardwareCommands.h"


#include "PluginAPI.h"

class Hardlight_Mk3_ZoneDriver {
public:
	CommandBuffer update(float dt);

	Hardlight_Mk3_ZoneDriver(Location area);

	Location GetLocation();

	bool IsPlaying();
	
	void consumeLasting(BasicHapticEventData data, ParentId);
	void controlEffect(ParentId handle, int command);
	//void realtime(uint16_t volume);
private:
	::Location m_area;

	ZoneModel m_retainedModel;
	RtpModel m_rtpModel;
	
	boost::uuids::random_generator m_gen;
	enum class Mode {Retained, Realtime};
	void transitionInto(Mode mode);
	Mode m_currentMode;
	std::mutex m_mutex;
	CommandBuffer m_commands;
};