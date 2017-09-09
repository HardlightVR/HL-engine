#include "stdafx.h"
#include "HardwarePlaybackController.h"

HardwarePlaybackController::HardwarePlaybackController(playback_api * api)
	: m_api(api)
	, m_cache()
{
}

void HardwarePlaybackController::CreateEventRecord(uint64_t request_id, std::vector<nsvr_node_id> nodes)
{

	auto& currentNodes = m_cache[request_id];
	currentNodes.insert(currentNodes.end(), nodes.begin(), nodes.end());

	std::sort(currentNodes.begin(), currentNodes.end());
	currentNodes.erase(std::unique(currentNodes.begin(), currentNodes.end()), currentNodes.end());

}


void HardwarePlaybackController::Cancel(uint64_t request_id)
{
	auto it = m_cache.find(request_id);
	if (it != m_cache.end()) {
		const auto& nodes = (*it).second;

		for (nsvr_node_id node : nodes) {
			m_api->submit_cancel(request_id, node);
		}
	}
}

void HardwarePlaybackController::Pause(uint64_t request_id)
{
	auto it = m_cache.find(request_id);
	if (it != m_cache.end()) {
		const auto& nodes = (*it).second;

		for (nsvr_node_id node : nodes) {
			m_api->submit_pause(request_id, node);
		}
	}
}

void HardwarePlaybackController::Resume(uint64_t request_id)
{
	auto it = m_cache.find(request_id);
	if (it != m_cache.end()) {
		const auto& nodes = (*it).second;

		for (nsvr_node_id node : nodes) {
			m_api->submit_unpause(request_id, node);
		}
	}
}
