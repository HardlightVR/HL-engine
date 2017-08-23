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
#include <bitset>
struct NamedRegion {
	nsvr_bodypart bodypart;
	SubRegionAllocation subregion;
	double segmentMin_offset;
	double segmentMax_offset;
	double rotationMin_radians;
	double rotationMax_radians;
	NamedRegion() : subregion(SubRegionAllocation::reserved_block_1) {}
	NamedRegion(SubRegionAllocation id, nsvr_bodypart b, double segmentMin_offset, double segmentMax_offset, double rotationMin_degrees, double rotationMax_degrees)
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
	static bool contains(SubRegionAllocation parentS, SubRegionAllocation childS, uint64_t* mag_difference) {
		
		if (parentS == childS) {
			return true;
		}
		uint64_t parent = static_cast<uint64_t>(parentS);
		uint64_t child = static_cast<uint64_t>(childS);
			
		uint64_t child_copy = child;
		uint64_t child_highbit = 0;

		while (child_copy >>= 1) {
			child_highbit++;
		}

		child_highbit -= 1;

		uint64_t parent_copy = parent;
		uint64_t parent_highbit = 0;

		while (parent_copy >>= 1) {
			parent_highbit++;
		}

		parent_highbit -= 1;


		if (child_highbit > parent_highbit) {
			return false;
		}

		*mag_difference = parent_highbit - child_highbit;

		const uint64_t block_size = uint64_t(1) << uint64_t(parent_highbit);
		uint64_t lower_bound = parent;
		uint64_t upper_bound = parent + block_size;
		return lower_bound < child && child <= upper_bound;
		

		//it is a child if:
		//1) it is the same or lower magnitude
		// AND 
		// 2) parent < child <= parent + blocksize
		
		
		// 3) OR they are the same



		
		
	}
	static bool contains(SubRegionAllocation parentS, SubRegionAllocation childS) {
		uint64_t throw_away;
		return contains(parentS, childS, &throw_away);
	}
	bool contains_subregion(SubRegionAllocation address, SubRegionAllocation* lowestAlloc) const{
		for (const auto& child : children) {
			if (child.contains_subregion(address, lowestAlloc)) {
				return true;
			}
		}
		uint64_t mag_dif;
		if (contains(this->subregion, address, &mag_dif)) {
			*lowestAlloc = this->subregion;
			return true;
		}
		return false;
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
	bool search(double r, double s, SubRegionAllocation* region)const {
		
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
	struct NodeData;
public:
	BodyGraph();
	int CreateNode(const char* name, nsvr_bodygraph_region* pose);
	int ConnectNodes(const char* a, const char* b);
	void Associate(const char * node, uint64_t device_id);
	void Unassociate(const char * node, uint64_t device_id);
	void ClearAssociations(uint64_t device_id);
	boost::optional<SubRegionAllocation> GetBestMatchingSubregion(SubRegionAllocation id);
	std::vector<uint64_t> GetDevicesForSubregion(SubRegionAllocation id);
private:
	
	boost::optional<SubRegionAllocation> findRegion(nsvr_bodypart bodypart, double r, double s);
	
	struct NodeData {
		std::string name;
		nsvr_bodygraph_region region;
		SubRegionAllocation mostSpecificRegion;
		std::vector<uint64_t> m_assocDevices;
		NodeData() : mostSpecificRegion(SubRegionAllocation::reserved_block_1){}
		NodeData(std::string name, nsvr_bodygraph_region region, SubRegionAllocation namedRegion) :
			name(name), region(region), mostSpecificRegion(namedRegion) {}
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