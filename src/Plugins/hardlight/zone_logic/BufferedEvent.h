#pragma once
#include <boost/uuid/uuid.hpp>
#include <stdint.h>
#include "PluginAPI.h"
#include <vector>

using ParentId = uint64_t;

class BufferedEvent {
public:
	BufferedEvent(ParentId handle, boost::uuids::uuid uniqueId, std::vector<double> data);
	std::vector<double> Data() const;
private:
	ParentId handle;
	boost::uuids::uuid uniqueId;
	std::vector<double> samples;
};