#pragma once
#include "PluginAPI.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/optional.hpp>
#include "BodyRegion.h"
#include "Enums.h"
#include "better_enum.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <bitset>


BETTER_ENUM(named_region, uint64_t,  
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
	identifier_head,
	identifier_palm_left,
	identifier_palm_right
);

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
	subregion* parent;
	std::vector<std::string> devices;
	subregion()
		: region(named_region::identifier_unknown)
		, seg{ 0, 0 }
		, ang{ 0,0 }
		, coords{ 0, 0, 0 }
		, children()
		, devices()
		, parent(nullptr) {}

	subregion(named_region region, segment_range segment_offset, angle_range angle_range)
		: region(region)
		, seg(segment_offset)
		, ang(angle_range)
		, children()
		, devices()
		, parent(nullptr) {
		calculateCoordinates();

	}

	subregion(named_region region, segment_range segment_offset, angle_range angle_range, std::vector<subregion> child_regions) 
		: region(region)
		, seg(segment_offset)
		, ang(angle_range)
		, children(std::move(child_regions))
		, devices()
		, parent(nullptr) {

		calculateCoordinates();


	}

	void init_backlinks() {
		for (subregion& child : children) {
			child.parent = this;
			child.init_backlinks();
		}
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

	subregion* find(named_region some_region) {
		if (this->region == some_region) {
			return this;
		}
		else {
			for (subregion& child : children) {
				subregion* ptr = child.find(some_region);
				if (ptr != nullptr) {
					return ptr;
				}
			}

			return nullptr;
		}
	}

	DistanceToRegion find_best_match(double segment_ratio, double angle_degrees) const {

		//We're searching for the closest named region. There's two possibilities:
		//Either this current one is the closest (case 1), or the closest is yet to be found, within the children (case 2).
		
		//When we declare the subregion data structure, we must make sure that each parent wholly encloses the 
		//children regions. We want to be able to say if !parent.contains(x,y), then for each child !child.contains(x,y). 
		
		//Note that the inverse is not necessarily true: a parent may contain a point that none of the children contain,
		//because we don't necessarily name all the regions inside a parent.
		

		// Case 1
		//So: if there's no children, then there's only one possibility at this level of recursion:
		//We're the closest.

		auto my_distance_info = std::make_pair(get_distance(segment_ratio, angle_degrees), region);

		if (children.empty()) {
			return my_distance_info;
		}


		// Case 2
		//Well, if there are children - we have the two possibilities on the table: this one's the closest,
		//or one of the children is, or one of the children's children.. etc.

		//aka best match = min(this, best_matches(all_children))
	
		//Also, what if we had a heuristic that could throw away whole subtrees if 
		//we knew they couldn't possibly contain the point? We do: .contains(x,y).

		std::vector<DistanceToRegion> candidates;
		for (const subregion& child : children) {
			if (child.contains(segment_ratio, angle_degrees)) {
				auto best_child_match = child.find_best_match(segment_ratio, angle_degrees);
				candidates.push_back(best_child_match);
			}
			else {
				//We should still include this child in case it is, in fact, the closest
				candidates.emplace_back(child.get_distance(segment_ratio, angle_degrees), child.region);
			}
		}


		auto least_distance = [](const DistanceToRegion& lhs, const DistanceToRegion& rhs) {
			return lhs.first < rhs.first;
		};

		auto best_of_children = std::min_element(candidates.begin(), candidates.end(), least_distance);
		return std::min(my_distance_info, *best_of_children, least_distance);	
	}


	
};


struct Bodypart {
	nsvr_bodypart bodypart;
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
	void Associate(const char * node, uint64_t device_id);
	void Unassociate(const char * node, uint64_t device_id);
	void ClearAssociations(uint64_t device_id);


	std::vector<uint64_t> getDevicesForNamedRegion(named_region region);
	//std::vector<uint64_t> getDevicesForCoordinate(double segment_ratio, double angle_degrees);
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