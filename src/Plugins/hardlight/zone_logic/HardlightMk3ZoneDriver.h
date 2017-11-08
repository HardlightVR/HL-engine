#pragma once
#include <mutex>
#include <boost/uuid/random_generator.hpp>

#include "Enums.h"

#include "ZoneModel.h"
#include "RtpModel.h"
#include "HardwareCommands.h"


#include "PluginAPI.h"

class Hardlight_Mk3_ZoneDriver {
public:
	CommandBuffer update(float dt);

	Hardlight_Mk3_ZoneDriver(Location area);

	Location GetLocation();

	bool IsPlaying();
	uint32_t GetId();
	void consumeLasting(BasicHapticEventData data, ParentId);
	void controlEffect(ParentId handle, int command);
	void realtime(std::vector<double> samples, ParentId);
private:
	::Location m_area;

	ZoneModel m_retainedModel;
	RtpModel m_rtpModel;
	
	boost::uuids::random_generator m_gen;
	enum class Mode {Continuous, Realtime};
	void transitionInto(Mode mode);
	Mode m_currentMode;
	std::mutex m_mutex;
	CommandBuffer m_commands;
};