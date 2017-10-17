#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_map>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/labeled_graph.hpp>

#include "BodyRegion.h"
#include "PluginAPI.h"
#include "SharedTypes.h"
#include "subregion.h"

struct Bodypart {
	nsvr_bodypart bodypart;

	//Unused for now, because we don't have any kind of calibration info
	double real_length;
	std::shared_ptr<subregion> region;
	Bodypart() 
		: bodypart(nsvr_bodypart_unknown)
		, real_length(0)
		, region() {}
	
	Bodypart(nsvr_bodypart b, double real_length, std::shared_ptr<subregion> r) :
		bodypart(b),
		real_length(real_length),
		region(r) {}
		
};

class BodyGraph {
	struct NodeData;
public:
	BodyGraph();
	int CreateNode(const char* name, nsvr_bodygraph_region* pose);
	int ConnectNodes(const char* a, const char* b);
	int Associate(const char * node, nsvr_node_id node_id);
	int Unassociate(const char * node, nsvr_node_id node_id);
	void ClearAssociations(nsvr_node_id node_id);


	std::vector<nsvr_node_id> getNodesForNamedRegion(subregion::shared_region region) const;
	std::vector<subregion::shared_region::_enumerated> getRegionsForNode(nsvr_node_id node) const;

	std::unordered_map<subregion::shared_region::_enumerated, std::vector<nsvr_node_id>> getAllNodes() const;
private:
	
	// This is the user facing data structure that hardware devs end up interfacing with.
	struct NodeData {
		std::string name;
		nsvr_bodygraph_region region;
		subregion::shared_region computed_region;
		std::vector<nsvr_node_id> nodes;

		NodeData() 
			: name()
			, region()
			, computed_region(subregion::shared_region::identifier_unknown) {}

		NodeData(std::string name, nsvr_bodygraph_region region, subregion::shared_region namedRegion) 
			: name(name)
			, region(region)
			, computed_region(namedRegion) {}
		
		void addNode(nsvr_node_id id);
		void removeNode(nsvr_node_id id);
	};

	using LabeledGraph = boost::labeled_graph<
		boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, NodeData>,
		std::string
	>;

	std::unordered_map<nsvr_bodypart, Bodypart> m_bodyparts;
	
	LabeledGraph m_nodes;
	
};