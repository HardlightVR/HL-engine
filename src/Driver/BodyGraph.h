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


enum named_region {
	identifier_unknown,
	identifier_body,
	identifier_torso,
	identifier_torso_front,
	identifier_chest_left,
	identifier_chest_right,
	identifier_upper_ab_left,
	identifier_middle_ab_left,
	identifier_lower_ab_left,
	identifier_upper_ab_right,
	identifier_middle_ab_right,
	identifier_lower_ab_right,
	identifier_torso_back,
	identifier_torso_left,
	identifier_torso_right,
	identifier_upper_back_left,
	identifier_upper_back_right,
	identifier_upper_arm_left,
	identifier_lower_arm_left,
	identifier_upper_arm_right,
	identifier_lower_arm_right,

	identifier_shoulder_left,
	identifier_shoulder_right,
	identifier_upper_leg_left,
	identifier_lower_leg_left,
	identifier_upper_leg_right,
	identifier_lower_leg_right,
	identifier_head
};

struct segment_range {
	double min;
	double max;
	static const segment_range full;
	static const segment_range lower_half;
	static const segment_range upper_half;
};


struct angle_range {
	double min;
	double max;
	static const angle_range full;
	static const angle_range left_half;
	static const angle_range right_half;
	static const angle_range front_half;
	static const angle_range back_half;
};


struct cartesian_barycenter {
	double x; //forward-back
	double y; //left-right
	double z; //height
};

double distance(double x, double y, double z, double x2, double y2, double z2);
double to_radians(double degrees);
double to_degrees(double radians);

struct subregion {
	named_region region;
	segment_range seg;
	angle_range ang;
	cartesian_barycenter coords;
	std::vector<subregion> children;

	subregion() 
		: region(named_region::identifier_unknown)
		, seg{ 0, 0 }
		, ang{ 0,0 }
		, coords{ 0, 0, 0 }
		, children{} {}

	subregion(named_region region, segment_range segment_offset, angle_range angle_range, std::vector<subregion> child_regions = std::vector<subregion>{}) 
		: region(region)
		, seg(segment_offset)
		, ang(angle_range)
		, children(child_regions) {

		calculateCoordinates();

	}

	void calculateCoordinates()
	{
		//https://en.wikipedia.org/wiki/List_of_centroids
		//Circular segment
		//The goal here is to calculate the barycenter of each of our pie slices
		//In hopes that doing a distance search from hardware-vendor-specified-point to the closest barycenter
		//will yield the closest region.

		//Unknown if this is true. It seems to work. 

		double alpha_rad = to_radians(angular_distance(ang.min, ang.max) / 2.0);

		//centroid calculation
		double initial_x = (2.0 * std::sin(alpha_rad)) / (3 * std::abs(alpha_rad));
		double initial_y = 0;

		//Since the circular segment may be rotated, we need to fix that. So we see how close it is from 0 and 360 (which are the same - forward)
		//and then correct by that angle.
		double dist_from_0 = 0 - average(ang.min, ang.max);
		double dist_from_360 = 360 - average(ang.min, ang.max);

		double angular_offset = 0;
		if (abs(dist_from_360) > abs(dist_from_0)) {
			angular_offset = to_radians(dist_from_0);
		}
		else {
			angular_offset = to_radians(dist_from_360);
		}

		//standard equations to rotate a point about a circle
		double x_rot = std::cos(angular_offset) * initial_x;
		double y_rot = std::sin(angular_offset) * initial_x;

		//center of Z is just the midpoint
		coords.z = 0.5 * (seg.max + seg.min);

		coords.x = x_rot;
		coords.y = y_rot;
	}


	static double average(double min, double max) {
		return (min + max) / 2.0;
	}

	static double angular_distance(double min, double max) {
		return max - min;
	}

	static bool is_between_deg(double value_deg, double min_deg, double max_deg) {
		if (min_deg < max_deg) {
			//pie slice
			return (min_deg <= value_deg && value_deg <= max_deg);
		}
		else {
			//you cut a pie slice, and they took the pie 
			//- Palmer
			return !(min_deg <= value_deg && value_deg <= max_deg);
		}
	}

	bool contains(double segment_ratio, double angle_degrees) const {
		return
			(seg.min <= segment_ratio && segment_ratio <= seg.max) &&
			(is_between_deg(angle_degrees, ang.min, ang.max));
	}

	double get_distance(double segment_ratio, double angle_degrees) const {
		double x = std::cos(to_radians(angle_degrees));
		//our Y axis is swapped in relation to a normal circle's. Our Y goes negative where it goes positive. Could correct this.
		double y = -std::sin(to_radians(angle_degrees));
		double z = segment_ratio;

		return distance(coords.x, coords.y, coords.z, x, y, z);
	}

	using DistanceToRegion = std::pair<double, named_region>;

	DistanceToRegion find_best_match(double segment_ratio, double angle_degrees) const {

		if (children.empty()) {
			return std::make_pair(get_distance(segment_ratio, angle_degrees), region);
		}

 		std::vector<DistanceToRegion> candidates;
		for (const subregion& child : children)
		{
			//If the child knows that it contains this point, then we should search deeper
			if (child.contains(segment_ratio, angle_degrees)) {
				candidates.push_back(child.find_best_match(segment_ratio, angle_degrees));
			}
		}

		//But, if no children definitely contained the point, then we should just compute their distances and select the best
		if (candidates.empty()) {
			for (const subregion& child : children)
			{
				candidates.emplace_back(child.get_distance(segment_ratio, angle_degrees), child.region);
			}
		}

		auto best_match = std::min_element(candidates.begin(), candidates.end(), [](const auto& d1, const auto& d2) { return d1.first < d2.first; });
		return *best_match;
	}
};

struct Bodypart {
	nsvr_bodypart bodypart;
	double real_length;
	subregion region;
	Bodypart() 
		: bodypart(nsvr_bodypart_unknown)
		, real_length(0)
		, region() {}
	
	Bodypart(nsvr_bodypart b, double real_length, subregion r) :
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
	void Associate(const char * node, uint64_t device_id);
	void Unassociate(const char * node, uint64_t device_id);
	void ClearAssociations(uint64_t device_id);
private:
	
	
	struct NodeData {
		std::string name;
		nsvr_bodygraph_region region;
		named_region computed_region;
		std::vector<uint64_t> devices;

		NodeData() 
			: name()
			, region()
			, computed_region(named_region::identifier_unknown) {}

		NodeData(std::string name, nsvr_bodygraph_region region, named_region namedRegion) 
			: name(name)
			, region(region)
			, computed_region(namedRegion) {}
		
		void addDevice(uint64_t id);
		void removeDevice(uint64_t id);
	};

	using LabeledGraph = boost::labeled_graph<
		boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, NodeData>,
		std::string
	>;

	std::unordered_map<nsvr_bodypart, Bodypart> m_bodyparts;

	LabeledGraph m_nodes;
	
};