#pragma once

#include "Enums.h"

#include "ZoneModel.h"
#include "RtpModel.h"
#include <boost/uuid/random_generator.hpp>
#include "HardwareCommands.h"



class Hardlight_Mk3_ZoneDriver {
public:
	CommandBuffer update(float dt);

	Hardlight_Mk3_ZoneDriver(Location area);

	Location GetLocation();

	//boost::optional<HapticDisplayInfo> QueryCurrentlyPlaying();

	void createOneshot(Location location, uint32_t effect, float strength);
	void createCont(uint64_t id, Location loc, uint32_t effect, float strength, float duration);
	void realtime(uint16_t volume);
private:
	::Location m_area;

	ZoneModel m_retainedModel;
	RtpModel m_rtpModel;
	
	boost::uuids::random_generator m_gen;
	enum class Mode {Retained, Realtime};
	void transitionInto(Mode mode);
	Mode m_currentMode;
	ParentId m_parentId;
	std::mutex m_mutex;
	CommandBuffer m_commands;
};