#pragma once
#include <boost/uuid/uuid.hpp>
#include <stdint.h>
#include "PluginAPI.h"




using ParentId = uint64_t;

class BasicHapticEventData {
public:
	uint32_t effect;
	uint32_t area;
	uint32_t repetitions;
	float strength;
};

class LiveBasicHapticEvent {
public:
	LiveBasicHapticEvent();
	LiveBasicHapticEvent(ParentId handle, boost::uuids::uuid uniqueId, BasicHapticEventData data);
	const BasicHapticEventData& PollOnce();
	bool operator==(const LiveBasicHapticEvent& other) const;
	void update(float dt);
	bool isFinished() const;
	bool isContinuous() const;
	bool isOneshot() const;
	bool isChildOf(const ParentId& handle) const;

private:
	ParentId handle;
	boost::uuids::uuid uniqueId;
	float currentTime;
	bool isPlaying;
	BasicHapticEventData eventData;
	uint32_t m_polls;
};