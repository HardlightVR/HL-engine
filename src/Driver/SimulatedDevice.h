#pragma once


#include "PluginApis.h"
#include "NodeView.h"
#include "SimulatedHapticNode.h"
#include "BodyGraphCreator.h"
#include "DeviceIds.h"
class SimulatedDevice {
public:
	SimulatedDevice(nsvr_device_id id, std::string name, std::string originatingPlugin, PluginApis& apis, std::shared_ptr<BodyGraphCreator> bodygraph);
	void  simulate(double dt);
	DeviceId<local> id() const;
	std::vector<NodeView> render();
	std::string name() const;
	std::string originatingPlugin() const;
private:
	void instrumentApis(PluginApis& apis);
	std::string m_name;
	std::string m_pluginName;
	std::shared_ptr<BodyGraphCreator> m_body;
	//Note...we have no region information..
	std::unordered_map<nsvr_node_id, SimulatedHapticNode> m_nodes;
	nsvr_device_id m_id;
};

