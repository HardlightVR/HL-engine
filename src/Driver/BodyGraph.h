#pragma once
#include "PluginAPI.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/optional.hpp>
#include "BodyRegion.h"
#include "Enums.h"

#define _USE_MATH_DEFINES
#include <math.h>
struct NamedRegion {
	nsvr_bodypart bodypart;
	SubRegionId subregion;
	double segmentMin_offset;
	double segmentMax_offset;
	double rotationMin_radians;
	double rotationMax_radians;
	NamedRegion() : subregion(SubRegionId::nsvr_region_unknown) {}
	NamedRegion(SubRegionId id, nsvr_bodypart b, double segmentMin_offset, double segmentMax_offset, double rotationMin_degrees, double rotationMax_degrees)
		: subregion(id)
		, bodypart(b)
		, segmentMin_offset(segmentMin_offset)
		, segmentMax_offset(segmentMax_offset)
	{
		assert(rotationMin_degrees >= 0);
		assert(rotationMax_degrees <= 360);
		assert(segmentMin_offset >= 0);
		assert(segmentMax_offset <= 1.0);

		rotationMin_radians = (rotationMin_degrees * M_PI) / 180.0;
		rotationMax_radians = (rotationMax_degrees * M_PI) / 180.0;

	}
	static bool between_deg(double val_deg, double min_deg, double max_deg)
	{
		return between((val_deg* M_PI) / 180.0, (min_deg* M_PI) / 180.0,
			(max_deg* M_PI) / 180.0);
	}
	static bool between(double val_radians, double min_radians, double max_radians) {

		if (max_radians > min_radians) {
			if (min_radians >= 0 && max_radians <= M_PI * 2) {
				return (val_radians >= min_radians && val_radians <= max_radians);

			}
			else {
				std::cout << "Inside between: value = " << val_radians << ", min = " << min_radians << ", max = " << max_radians << "\n";
				return true;
			}
		}
		else {
			return ((val_radians >= 0 && val_radians <= max_radians) || (val_radians >= min_radians && val_radians <= M_PI * 2));
			//crossing over 0 boundary
		}

	
		
	}
	
	bool contains(double segment_offset, double rotation_degrees) const {
		double normalized_degrees = 0;
		if (rotation_degrees < 0) { 
			normalized_degrees = 360 + rotation_degrees; 
		} else { 
			normalized_degrees = rotation_degrees;
		}
		double normalized_radians = (normalized_degrees * M_PI) / 180.0;
		return (segment_offset >= segmentMin_offset && segment_offset <= segmentMax_offset 
			&& between(normalized_radians, this->rotationMin_radians, this->rotationMax_radians));
	}
	bool search(double r, double s, SubRegionId* region)const {
		if (this->subregion == 121000) {
			int z = 12;
		}
		for (const auto& child : children) {
			if (child.search(r, s, region)) {
				return true;
			}
		}

		if (contains(r, s)) {
			*region = this->subregion;
			return true;
		}

		return false;
	}
	std::vector<NamedRegion> children;
};

struct SubRegion {
	std::size_t r_coord;
	std::size_t s_coord;
	double parallelMin;
	double parallelMax;
	double rotationMin;
	double rotationMax;

	//temporary until better solution
	double real_segment_length;
	SubRegion() {}
	SubRegion(std::size_t r, std::size_t s, double paraMin, double paraMax, double rotMin, double rotMax, double seg_length) :
		r_coord(r),
		s_coord(s),
		parallelMin(paraMin),
		parallelMax(paraMax),
		rotationMin(rotMin),
		rotationMax(rotMax),
		real_segment_length(seg_length) {}

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

			//todo: return something actually useful
			*result = SubRegionId::nsvr_region_unknown;
			return true;
		}
		else {
			//std::cout << "		Nope, no result\n";

			return false;
		}
	}
};
struct Bodypart {
	nsvr_bodypart bodypart;
	double real_length;
	SubRegion region;
	Bodypart() {}
	Bodypart(nsvr_bodypart b, double real_length) :
		bodypart(b),
		real_length(real_length),
		region(0,0,0,1,0,360,0){}
};

class BodyGraph {
public:
	BodyGraph();
	int CreateNode(const char* name, nsvr_bodygraph_region* pose);
	int ConnectNodes(const char* a, const char* b);
	void Associate(const char * node, uint64_t device_id);
	void Unassociate(const char * node, uint64_t device_id);
	void ClearAssociations(uint64_t device_id);
private:
	
	boost::optional<SubRegionId> findRegion(nsvr_bodypart bodypart, double r, double s);
	
	struct NodeData {
		std::string name;
		nsvr_bodygraph_region region;
		NamedRegion foundRegion;
		std::vector<uint64_t> m_assocDevices;
		NodeData() {}
		NodeData(std::string name, nsvr_bodygraph_region region, NamedRegion namedRegion) :
			name(name), region(region), foundRegion(namedRegion) {}
		void addDevice(uint64_t id);
		void removeDevice(uint64_t id);
	};
	using LabeledGraph = boost::labeled_graph<
		boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, NodeData>,
		std::string
	>;
	using Graph = boost::undirected_graph<NodeData>;
	std::unordered_map<nsvr_bodypart, Bodypart> m_bodyparts;

	std::unordered_map<nsvr_bodypart, NamedRegion> m_namedRegions;

	LabeledGraph m_nodes;
	
};