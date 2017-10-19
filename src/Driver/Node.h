#pragma once
#include <string>
#include "PluginAPI.h"
#include "DeviceIds.h"
struct NodeDescriptor {
	nsvr_node_concept type;
	std::string displayName;
	nsvr_node_id id;
};

class Node {
public:
	Node(const NodeDescriptor&);
	Node();
	NodeId<local> id() const;
	std::string name() const;
	nsvr_node_concept type() const;
private:
	std::string m_name;
	nsvr_node_id m_id;
	nsvr_node_concept m_type;
};