#include "stdafx.h"
#include "LiveBasicHapticEvent.h"
#include <cmath>


LiveBasicHapticEvent::LiveBasicHapticEvent() :
	handle(0),
	uniqueId(),
	currentTime(0),
	isPlaying(false),
	eventData(),
	m_polls(0),
	m_duration(0)
{
	eventData.repetitions = 1;
}

LiveBasicHapticEvent::LiveBasicHapticEvent(ParentId  handle, boost::uuids::uuid uniqueId, BasicHapticEventData data, std::chrono::milliseconds duration) :
	handle(handle),
	uniqueId(uniqueId),
	currentTime(0),
	isPlaying(true),
	eventData(std::move(data)),
	m_polls(0),
	m_duration(duration)

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
	auto elapsedMs = static_cast<int>(1000 * currentTime); // .1 seconds elapsed = 100 ms

	return m_polls > eventData.repetitions && (std::chrono::milliseconds(elapsedMs) >=m_duration); //todo: here is where we absolutely need to use the correct time for each waveform. 
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

std::chrono::milliseconds LiveBasicHapticEvent::Duration() const
{
	return m_duration;
}


