#include "stdafx.h"
#include "BufferedEvent.h"

BufferedEvent::BufferedEvent(ParentId handle, boost::uuids::uuid uniqueId, std::vector<double> data)
	: handle(handle)
	, uniqueId(uniqueId)
	, samples(std::move(data))
{
}

std::vector<double> BufferedEvent::Data() const
{
	return samples;
}
