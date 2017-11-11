#include "stdafx.h"
#include "LiveBasicHapticEvent.h"
#include <cmath>


LiveBasicHapticEvent::LiveBasicHapticEvent() :
	handle(0),
	uniqueId(),
	currentTime(0),
	isPlaying(false),
	eventData(),
	m_polls(0)
{
	eventData.repetitions = 1;
}

LiveBasicHapticEvent::LiveBasicHapticEvent(ParentId  handle, boost::uuids::uuid uniqueId, BasicHapticEventData data) :
	handle(handle),
	uniqueId(uniqueId),
	currentTime(0),
	isPlaying(true),
	eventData(std::move(data)),
	m_polls(0)

{
	if (data.repetitions == 0) {
		eventData.repetitions = 1;
	}
}

const BasicHapticEventData & LiveBasicHapticEvent::PollOnce()
{
	m_polls++;
	return eventData;
}

bool LiveBasicHapticEvent::operator==(const LiveBasicHapticEvent & other) const
{
	return uniqueId == other.uniqueId;
}

void LiveBasicHapticEvent::update(float dt)
{
	if (isPlaying) {
		currentTime += dt;
	}
}

bool LiveBasicHapticEvent::isFinished() const
{
	return m_polls >= eventData.repetitions && currentTime >= 0.1f; //todo: here is where we absolutely need to use the correct time for each waveform. 
}

bool LiveBasicHapticEvent::isContinuous() const
{
	return !isOneshot();
}

bool LiveBasicHapticEvent::isOneshot() const
{
	return eventData.repetitions == 1;
}

bool LiveBasicHapticEvent::isChildOf(const ParentId& handle) const
{
	return this->handle == handle;
	//return this->parentId == parentId;
}


