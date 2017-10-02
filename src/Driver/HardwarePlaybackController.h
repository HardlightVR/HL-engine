#pragma once
#include "PlaybackController.h"
#include "PluginApis.h"

#include <unordered_map>
#include <vector>

class HardwarePlaybackController : public PlaybackController {
public:
	HardwarePlaybackController(playback_api* api);
	void CreateEventRecord(uint64_t request_id, std::vector<NodeId<local>> nodes) override;
	void Cancel(uint64_t request_id) override;
	void Pause(uint64_t request_id) override;
	void Resume(uint64_t request_id) override;
private:
	playback_api* m_api;
	std::unordered_map<uint64_t, std::vector<NodeId<local>>> m_cache;
};