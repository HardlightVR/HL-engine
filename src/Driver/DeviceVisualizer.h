#pragma once

#include <memory>


#include "RenderedRegion.h"
#include "SimulatedHapticNode.h"
#include "PluginApis.h"

class DeviceVisualizer {
public:
	void provideApi(waveform_api* api);
	void provideApi(playback_api* api);
	void simulate(double dt);
	std::vector<RenderedNode> render();
private:

	std::unordered_map<nsvr_node_id, SimulatedHapticNode> m_nodes;

};