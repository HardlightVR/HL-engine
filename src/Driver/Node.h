#pragma once
#include <string>
#include "PluginAPI.h"

struct NodeDescriptor {
	nsvr_node_type type;
	std::string displayName;
	nsvr_node_id id;
};

class Node {
public:
	Node(const NodeDescriptor&);
	Node();
	nsvr_node_id id() const;
	std::string name() const;
	nsvr_node_type type() const;
private:
	std::string m_name;
	nsvr_node_id m_id;
	nsvr_node_type m_type;
};