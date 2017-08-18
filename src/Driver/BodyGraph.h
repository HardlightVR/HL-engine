#pragma once
#include "PluginAPI.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/labeled_graph.hpp>
#include "BodyRegion.h"
class BodyGraph {
public:
	int CreateNode(const char* name, nsvr_bodygraph_region* pose);
	int CreateNodeRelative(const char* a, nsvr_region_relation relation, const char* B, double offset);
	int ConnectNodes(const char* a, const char* b);
private:
	struct NodeData {
		std::string name;
		nsvr_bodygraph_region region;
	};
	using LabeledGraph = boost::labeled_graph<
		boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, NodeData>,
		std::string
	>;
	using Graph = boost::undirected_graph<NodeData>;

	LabeledGraph m_nodes;
	
};