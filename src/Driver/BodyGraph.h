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


enum placeholder {
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
	identifier_lower_arm_right
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
	double x_radial;
	double y;
	double z_height;
};

double distance(double x, double y, double z, double x2, double y2, double z2);

struct subregion {
	placeholder p;
	segment_range seg;
	angle_range ang;
	cartesian_barycenter coords;
	std::vector<subregion> children;

	subregion() : p(placeholder::identifier_unknown), seg{ 0, 0 }, ang{ 0,0 }, coords{ 0,0,0 }, children{} {}
	subregion(placeholder p, segment_range s, angle_range a, std::vector<subregion> c = std::vector<subregion>{}) :
		p(p), seg(s), ang(a), children(c) {



		double alpha_rad = to_rad(angular_distance(ang.min, ang.max) / 2.0);
		double initial_x = (2.0 * std::sin(alpha_rad)) / (3 * std::abs(alpha_rad));
		double initial_y = 0;

		double dist_from_0 = 0 - angular_average(ang.min, ang.max);
		double dist_from_360 = 360 - angular_average(ang.min, ang.max);

		double angular_offset_from_0 = 0;
		if (abs(dist_from_360) > abs(dist_from_0)) {
			angular_offset_from_0 = to_rad(dist_from_0);
		}
		else {
			angular_offset_from_0 = to_rad(dist_from_360);
		}

		double x_rot = std::cos(angular_offset_from_0) * initial_x;
		double y_rot = std::sin(angular_offset_from_0) * initial_x;

		coords.z_height = 0.5 * (s.max + s.min);
		coords.x_radial = x_rot;
		coords.y = y_rot;
	}

	static double to_rad(double degrees) {
		return (degrees * M_PI) / 180.0;
	}
	static double angular_average(double min, double max) {
		double avg = (min + max) / 2.0;
		return avg;
	}
	static double angular_distance(double min, double max) {
		double dif = max - min;
		
		return dif;
	}


	static bool between_deg(double value_deg, double min_deg, double max_deg) {

	
		if (min_deg < max_deg) {
		
			return (min_deg <= value_deg && value_deg <= max_deg);
		}
		else {
			return !(min_deg <= value_deg && value_deg <= max_deg);
		}


	}
	bool contains(double segment_ratio, double angle_degrees) const {
		return
			(seg.min <= segment_ratio && segment_ratio <= seg.max) &&
			(between_deg(angle_degrees, ang.min, ang.max));
	}

	double get_distance(double segment_ratio, double angle_degrees) const {
		double x = std::cos(to_rad(angle_degrees));
		double y = -std::sin(to_rad(angle_degrees));
		double z = segment_ratio;

		return distance(coords.x_radial, coords.y, coords.z_height, x, y, z);
	}

	using DistanceToRegion = std::pair<double, placeholder>;
	DistanceToRegion find_best_match(double segment_ratio, double angle_degrees) const {

		if (children.empty()) {
			return std::make_pair(get_distance(segment_ratio, angle_degrees), p);
		}

 		std::vector<DistanceToRegion> candidates;
		for (const subregion& child : children)
		{
			//heuristic
			if (child.contains(segment_ratio, angle_degrees)) {
				candidates.push_back(child.find_best_match(segment_ratio, angle_degrees));
			}
		}

		//don't bother searching the children deeper if they don't contain it
		if (candidates.empty()) {
			for (const subregion& child : children)
			{
				candidates.emplace_back(child.get_distance(segment_ratio, angle_degrees), child.p);
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
	Bodypart() : bodypart(nsvr_bodypart_unknown), real_length(0), region() {}
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


	LabeledGraph m_nodes;
	
};