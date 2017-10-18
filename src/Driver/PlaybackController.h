#pragma once
#include <cstdint>
#include "PluginAPI.h"
#include <vector>

class PlaybackController {

public:
	virtual ~PlaybackController() {}

	virtual void CreateEventRecord(uint64_t request_id, std::vector<nsvr_node_id> nodes) = 0;
	virtual void Cancel(uint64_t request_id) = 0;
	virtual void Pause(uint64_t request_id) = 0;
	virtual void Resume(uint64_t request_id) = 0;
};