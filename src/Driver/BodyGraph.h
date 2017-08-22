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
	int ConnectNodes(const char* a, const char* b);
	void Associate(const char * node, uint64_t device_id);
	void Unassociate(const char * node, uint64_t device_id);
	void ClearAssociations(uint64_t device_id);
private:
	struct SubRegion {
		SubRegionId region;
		double parallelMin;
		double parallelMax;
		double rotationMin;
		double rotationMax;

		//temporary until better solution
		double real_segment_length;

		SubRegion() :region(SubRegionId::nsvr_region_unknown){}
		SubRegion(SubRegionId region, double paraMin, double paraMax, double rotMin, double rotMax, double seg_length) :
			region(region), 
			parallelMin(paraMin), 
			parallelMax(paraMax), 
			rotationMin(rotMin), 
			rotationMax(rotMax), 
			real_segment_length(seg_length) {}

		SubRegion(SubRegionId region): 
			region(region), 
			parallelMin(0), 
			parallelMax(1), 
			rotationMin(-360), 
			rotationMax(360), 
			real_segment_length(0) {}

		std::vector<SubRegion> children;
		bool contains(double parallel, double rotation) {
			return (parallel >= parallelMin && parallel <= parallelMax) &&
				(rotation >= rotationMin && rotation <= rotationMax);
		}

		bool search(double parallel, double rotation, SubRegionId* result) {
			for (auto& child : children) {
			//	std::cout << "		Searching child " << (+child.region)._to_string() << '\n';
				if (child.search(parallel, rotation, result)) {
				//	std::cout << "		Yup, it contained it\n";
					return true;
				}
			}

			if (contains(parallel, rotation)) {
				//std::cout << "		I contain it!\n";

				*result = region;
				return true;
			}
			else {
				//std::cout << "		Nope, no result\n";

				return false;
			}
		}
	};
	
	struct NodeData {
		std::string name;
		nsvr_bodygraph_region region;
		SubRegion foundRegion;
		std::vector<uint64_t> m_assocDevices;

		void addDevice(uint64_t id);
		void removeDevice(uint64_t id);
	};
	using LabeledGraph = boost::labeled_graph<
		boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, NodeData>,
		std::string
	>;
	using Graph = boost::undirected_graph<NodeData>;
	std::unordered_map<nsvr_bodypart, SubRegion> m_namedRegions;
	LabeledGraph m_nodes;
	
};