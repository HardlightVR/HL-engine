#pragma once
#include "PluginAPI.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/labeled_graph.hpp>
#include "BodyRegion.h"
#include "Enums.h"
class BodyGraph {
public:
	BodyGraph();
	int CreateNode(const char* name, nsvr_bodygraph_region* pose);
	int CreateNodeRelative(const char* a, nsvr_region_relation relation, const char* B, double offset);
	int ConnectNodes(const char* a, const char* b);
private:
	struct SubRegion {
		SubRegionId region;
		double parallelMin;
		double parallelMax;
		double rotationMin;
		double rotationMax;
		SubRegion() :region(SubRegionId::nsvr_region_unknown){}
		SubRegion(SubRegionId region, double paraMin, double paraMax, double rotMin, double rotMax) :
			region(region), parallelMin(paraMin), parallelMax(paraMax), rotationMin(rotMin), rotationMax(rotMax) {}

		SubRegion(SubRegionId region): 
			region(region), parallelMin(0), parallelMax(1), rotationMin(-360), rotationMax(360) {}

		std::vector<SubRegion> children;
		bool contains(double parallel, double rotation) {
			return (parallel >= parallelMin && parallel <= parallelMax) &&
				(rotation >= rotationMin && rotation <= rotationMax);
		}

		bool search(double parallel, double rotation, SubRegionId* result) {
			for (auto& child : children) {
				if (child.search(parallel, rotation, result)) {
					return true;
				}
			}

			if (contains(parallel, rotation)) {
				*result = region;
				return true;
			}
			else {
				return false;
			}
		}
	};
	
	struct NodeData {
		std::string name;
		nsvr_bodygraph_region region;
	};
	using LabeledGraph = boost::labeled_graph<
		boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, NodeData>,
		std::string
	>;
	using Graph = boost::undirected_graph<NodeData>;
	std::unordered_map<nsvr_bodypart, SubRegion> m_namedRegions;
	LabeledGraph m_nodes;
	
};