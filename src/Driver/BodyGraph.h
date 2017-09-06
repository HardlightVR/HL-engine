// Copyright (c) 2011 rubicon IT GmbH
#pragma once
#include "PluginAPI.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/optional.hpp>
#include "BodyRegion.h"
#include "Enums.h"
#include "better_enum.h"

#include "SharedTypes.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <bitset>


///
/// Represents a range of values, from min to max, in the context of a body segment.
/// For example, on the lower arm, you might have a segment that ranges from 0-1, which is the entire 
/// lower arm. You may then have a segment contained within that, which goes from 0.5-1, representing the top half 
/// of the lower arm.
struct segment_range {
	double min;
	double max;
	static const segment_range full;
	static const segment_range lower_half;
	static const segment_range upper_half;
};

///
/// Represents an angular section of a body segment, from min to max in degrees. The values
/// should always be specified from min to max, that is, from 0-360, where 0 && 360 represent dead ahead of the body.
/// If you need to specify something like the chest, just do 350-10, don't use negative values. 
struct angle_range {
	double min;
	double max;
	static const angle_range full;
	static const angle_range left_half;
	static const angle_range right_half;
	static const angle_range front_half;
	static const angle_range back_half;
};


///
/// Represents the center of a body segment. Imagine these segments as pie-wedge shaped sections, all the way
/// up to a full pie. The barycenter is the center of mass of one of these segments.
struct cartesian_barycenter {
	double x; //forward-back
	double y; //left-right
	double z; //height
};

double distance(double x, double y, double z, double x2, double y2, double z2);
double to_radians(double degrees);
double to_degrees(double radians);


///
/// Subregions, when combined, create a hierarchical definition of the human body. 
/// For each independently moving body part of the body, we define a subregion which contains the entire part.
/// For example, lower_arm, upper_arm, lower_leg, etc. These subregions can then contain child regions which more specifically
/// divide up the region. Each child subregion must be wholly contained in the parent, but it's not necessary for all the children to 
/// completely cover the parent region. This flexibility allows us to specify useful areas, and if a hardware dev asks for something not within
/// those areas, the core will select the next best thing.
struct subregion {
	using shared_region = NullSpace::SharedMemory::nsvr_shared_region;

	// The named region that we have assigned to this subregion
	shared_region region;

	segment_range seg;
	angle_range ang;

	cartesian_barycenter coords;

	// Any child subregions are listed here
	std::vector<subregion> children;

	// Back pointer to parent, necessary to traverse the hierarchy upwards
	subregion* parent;

	std::vector<std::string> hardware_defined_regions;
	
	
	subregion();

	subregion(shared_region region, segment_range segment_offset, angle_range angle_range);

	subregion(shared_region region, segment_range segment_offset, angle_range angle_range, std::vector<subregion> child_regions);

	// Patches up the hierarchy, linking each child to the parent. This should only be called after the entire hierarchy is setup
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
			//think pie slice
			return (min_deg <= value_deg && value_deg <= max_deg);
		}
		else {
			//think: you cut a pie slice, and they took the pie 
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

	using DistanceToRegion = std::pair<double, shared_region>;

	// Find the subregion corresponding to the given named region. Returns nullptr if no matching subregion is found.
	subregion* find(shared_region some_region) {
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

	// Finds the closest named region corresponding to a set of coordinates given by the segment ratio and angle, 
	DistanceToRegion find_best_match(double segment_ratio, double angle_degrees) const {

		//We're searching for the closest named region. There's two possibilities:
		//Either this current one is the closest (case 1), or the closest is yet to be found, within the children (case 2).
		
		//When we declare the subregion data structure, we must make sure that each parent wholly encloses the 
		//children regions. We want to be able to say if !parent.contains(x,y), then for each child !child.contains(x,y). 
		
		//Note that the inverse is not necessarily true: a parent may contain a point that none of the children contain,
		//because we don't necessarily name all the regions inside a parent.
		

		// Case 1
		//If there's no children, then there's only one possibility at this level of recursion:
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
				//We just won't search its children, because they can't possibly contain the point.
				//But now that I think of it, it could be that a child actually is closer than the parent, because
				//its barycenter is closer. So actually we probably should still fully search each child..
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
	void Associate(const char * node, nsvr_node_id node_id);
	void Unassociate(const char * node, nsvr_node_id node_id);
	void ClearAssociations(nsvr_node_id node_id);


	std::vector<nsvr_node_id> getNodesForNamedRegion(subregion::shared_region region);


	std::unordered_map<subregion::shared_region, std::vector<nsvr_node_id>> getAllNodes() const;
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