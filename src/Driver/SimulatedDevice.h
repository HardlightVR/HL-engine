#pragma once


#include "PluginApis.h"
#include "NodeView.h"
#include "SimulatedHapticNode.h"
#include "BodyGraphCreator.h"
class SimulatedDevice {
public:
	SimulatedDevice(nsvr_device_id id, PluginApis& apis, std::shared_ptr<BodyGraphCreator> bodygraph);
	void  simulate(double dt);
	nsvr_device_id id() const;
	std::vector<NodeView> render();
private:
	void instrumentApis(PluginApis& apis);

	std::shared_ptr<BodyGraphCreator> m_body;
	//Note...we have no region information..
	std::unordered_map<nsvr_node_id, SimulatedHapticNode> m_nodes;
	nsvr_device_id m_id;
};

