#pragma once
#include <string>
#include <unordered_set>
#include "PluginAPI.h"
#include "PluginApis.h"
#include "DeviceIds.h"
struct NodeDescriptor {
	nsvr_node_concept type;
	std::string displayName;
	nsvr_node_id id;
	nsvr_api_support apiSupport;
};

class Node {
public:
	Node(const NodeDescriptor&);
	Node();
	NodeId<local> id() const;
	std::string name() const;
	nsvr_node_concept type() const;

	template<typename Api>
	bool supports() const;
private:
	std::unordered_set<Apis::_enumerated> m_support;
	std::string m_name;
	nsvr_node_id m_id;
	nsvr_node_concept m_type;
};

template<typename Api>
inline bool Node::supports() const
{
	return m_support.find(Api::getApiType()) != m_support.end();
}
