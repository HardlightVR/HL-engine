#include "stdafx.h"
#include "BodyGraph.h"
#include <boost/graph/iteration_macros.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

void subdivide(SubRegion& region, std::size_t num_rings, std::size_t num_strips) {
	const double ring_width = region.real_segment_length / num_rings;
	const double strip_arclength = (360) / num_strips;
	for (std::size_t ring = 0; ring < num_rings; ring++) {
		for (std::size_t strip = 0; strip < num_strips; strip++) {
			region.children.push_back(SubRegion(
				ring, strip,
				ring * ring_width,
				(ring * ring_width) + ring_width,
				strip * strip_arclength,
				(strip * strip_arclength) + strip_arclength,
				ring_width
			));
		}
	}
}

void subdivide(Bodypart& bp, std::size_t num_rings, std::size_t num_strips) {
	subdivide(bp.region, num_rings, num_strips);
}
enum placeholder {
	identifier_unknown,
	identifier_torso_front,
	identifier_chest_left,
	identifier_chest_right,
	identifier_upper_ab_left,
	identifier_middle_ab_left,
	identifier_lower_ab_left,
	identifier_torso_back,
 identifier_torso_left ,
 identifier_torso_right
};

struct segment_range {
	double min;
	double max;
	static const segment_range full;
	static const segment_range lower_half;
	static const segment_range upper_half;

};
const segment_range segment_range::full = { 0, 1 };
const segment_range segment_range::lower_half = { 0, 0.5 };
const segment_range segment_range::upper_half = { 0.5, 1.0 };

struct angle_range {
	double min;
	double max;

	static const angle_range full;
	static const angle_range left_half;
	static const angle_range right_half;
	static const angle_range front_half;
	static const angle_range back_half;
};

const angle_range angle_range::full = { 0, 360 };
const angle_range angle_range::left_half = { 180, 360 };
const angle_range angle_range::right_half = { 0, 180 };
const angle_range angle_range::front_half = { 270, 90 };
const angle_range angle_range::back_half = { 90, 270 };
struct cartesian_barycenter {
	double x_radial;
	double y;
	double z_height;
};
struct subregion {
	placeholder p;
	segment_range seg;
	angle_range ang;
	cartesian_barycenter coords;
	std::vector<subregion> children;
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
		while (dif < -180) dif += 360;
		while (dif > 180) dif -= 360;
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
	static double distance(double x, double y, double z, double x2, double y2, double z2) {
		return sqrt(pow((x - x2), 2) + pow((y - y2), 2) + pow((z - z2), 2));
	}
	double get_distance(double x, double y, double z) const {
		return distance(x, y, z, coords.x_radial, coords.y, coords.z_height);
	}
	void find_best_match(double segment_ratio, double angle_degrees, placeholder* outP, double* outDist) const {
	
		//if have children, see if one or multiple ones contain it.
		//if >1 contain it then resolve with a distance check
		

		double x = std::cos(to_rad(angle_degrees));
		double y = std::sin(to_rad(angle_degrees));
		double z = segment_ratio;

		for (const subregion& child : children) {
			placeholder best_child = placeholder::identifier_unknown;
			double best_dist = std::numeric_limits<double>::max();
			if (child.contains(segment_ratio, angle_degrees)) {
				child.find_best_match(segment_ratio, angle_degrees, outP, outDist);
				if (*outDist < best_dist) {
					best_dist = *outDist;
					best_child = *outP;
				}
			}
			*outP = best_child;
			*outDist = best_dist;
		}

		double myDist = get_distance(x, y, z);
		if (myDist < *outDist) {
			*outP = p;
			*outDist = myDist;
		}
		
		

	



		
	}
};

struct describe {
	nsvr_bodypart bodypart;
	std::vector<subregion> children;

	placeholder find_best_match(double segment_ratio, double angle_degrees) {
		placeholder search_results = placeholder::identifier_unknown;
		double dist = std::numeric_limits<double>::max();
		for (const subregion& child : children) {
			child.find_best_match(segment_ratio, angle_degrees, &search_results, &dist);
		}
		return search_results;
	}

};
BodyGraph::BodyGraph()
{


	auto entire_torso = 
	describe{ nsvr_bodypart_torso, {
		subregion(identifier_torso_front, segment_range::full, angle_range::front_half, 
			{
				subregion(identifier_chest_left,  segment_range{0.8, 1.0}, angle_range{350, 0}),
				subregion(identifier_upper_ab_left,  segment_range{ 0.6, 0.8 }, angle_range{ 350, 0 }),
				subregion(identifier_middle_ab_left, segment_range{0.4, 0.6}, angle_range{350, 0}),
				subregion(identifier_lower_ab_left,  segment_range{0.0, 0.4}, angle_range{350, 0})
			}
		),
		subregion(identifier_torso_back,
			segment_range::full,
			angle_range::back_half
		),
		subregion(identifier_torso_left, 
			segment_range::full,
			angle_range::left_half
		),
		subregion(identifier_torso_right,
			segment_range::full,
			angle_range::right_half
		)
	}};

	
	auto named_results = entire_torso.find_best_match(0.5, 0);
	auto named_results3 = entire_torso.find_best_match(0.5, 180);

	auto named_results2 = entire_torso.find_best_match(0.8, 354);

	Bodypart upperArmLeft{ nsvr_bodypart_upperarm_left , 29.41 };
	//subdivide(upperArmLeft, 10, 10);
	m_bodyparts[nsvr_bodypart_upperarm_left] = upperArmLeft;


	Bodypart lowerArmLeft{ nsvr_bodypart_forearm_left, 27.68 };
	//subdivide(lowerArmLeft, 10, 10);
	m_bodyparts[nsvr_bodypart_forearm_left] = lowerArmLeft;


	Bodypart upperLegLeft{ nsvr_bodypart_upperleg_left, 43.25 };
	m_bodyparts[nsvr_bodypart_upperleg_left] = upperLegLeft;

	Bodypart lowerLegLeft{ nsvr_bodypart_lowerleg_left, 43.25 };
	m_bodyparts[nsvr_bodypart_lowerleg_left] = lowerLegLeft;

	Bodypart torso{ nsvr_bodypart_torso, 51.9 };
	/*torso.children = {
		SubRegion { SubRegionId::nsvr_region_chest_front_left, .75, 1.0, -75, 0 , 15},
		SubRegion { SubRegionId::nsvr_region_ab_upper_left, .55, .75, -75, 0,  12.3},
		SubRegion { SubRegionId::nsvr_region_ab_middle_left , .45, .55, -75, 0, 12.3},
		SubRegion { SubRegionId::nsvr_region_ab_lower_left, 0, .45, -75, 0, 12.3}
	};*/
	//subdivide(torso, 20, 10);
	m_bodyparts[nsvr_bodypart_torso] = torso;

	Bodypart head{ nsvr_bodypart_head, 19.03 };
	m_bodyparts[nsvr_bodypart_head] = head;

	Bodypart hips{ nsvr_bodypart_hips, 12.11 };
	m_bodyparts[nsvr_bodypart_hips] = hips;



	//todo: fix the SubRegionAllocations to be more specific
	m_namedRegions[nsvr_bodypart_torso].children = {
		//chest
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, .75, 1.0, 350, 0),
		//upper ab
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.50, 0.75, 350, 0),
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.25, 0.50, 350, 0),
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.0, 0.25, 350, 0),
		//back
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.0, 1.0, 180, 270),

		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, .75, 1.0, 0, 10),
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.50, 0.75, 0, 10),
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.25, 0.50, 0, 10),
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.0, 0.25, 0, 10),
		NamedRegion(SubRegionAllocation::torso_front, nsvr_bodypart_torso, 0.0, 1.0, 90, 180)
	};

	m_namedRegions[nsvr_bodypart_upperarm_left].children = {
		NamedRegion(SubRegionAllocation::arm_left, nsvr_bodypart_upperarm_left, 0.90, 1.0, 0, 360),
		NamedRegion(SubRegionAllocation::arm_left, nsvr_bodypart_upperarm_left, 0.0, 0.90, 0, 360)
	};
	m_namedRegions[nsvr_bodypart_upperarm_right].children = {
		//shoulder
		NamedRegion(SubRegionAllocation::arm_right, nsvr_bodypart_upperarm_right, 0.90, 1.0, 0, 360),
		//upper arm
		NamedRegion(SubRegionAllocation::arm_right, nsvr_bodypart_upperarm_right, 0.0, 0.90, 0, 360)
	};


	m_namedRegions[nsvr_bodypart_forearm_left].children = {
		NamedRegion(SubRegionAllocation::arm_left, nsvr_bodypart_forearm_left, 0.0, 1.0, 0, 360)
	};

	m_namedRegions[nsvr_bodypart_forearm_right].children = {
		NamedRegion(SubRegionAllocation::arm_right, nsvr_bodypart_forearm_right, 0.0, 1.0, 0, 360)
	};
}



int BodyGraph::CreateNode(const char * name, nsvr_bodygraph_region * pose)
{
	boost::add_vertex(name, m_nodes);
	SubRegionAllocation id = SubRegionAllocation::reserved_block_1;
	
	std::cout << "[" << name << "] Searching for bodypart=" << pose->bodypart << ", " << pose->rotation << "\n";

	if (auto region = findRegion(pose->bodypart, pose->segment_ratio, pose->rotation)) {
		//id = *region;
		//std::cout << "	Found a valid region: " << (+*region)._to_string() << "\n";
		id = *region;
		//todo: fix for subregion alloc

	}
	else {
		std::cout << "	Didn't find any regions matching that.\n";
	}
	//todo: fix for subregion alloc
	m_nodes[name] = NodeData( name, *pose, id);




	return 0;
}


int BodyGraph::ConnectNodes(const char* a, const char* b)
{
	boost::add_edge_by_label(a, b, m_nodes);
	return 0;
}

void BodyGraph::Associate(const char * node, uint64_t device_id)
{
	m_nodes[node].addDevice(device_id);
}

void BodyGraph::Unassociate(const char * node, uint64_t device_id)
{
	m_nodes[node].removeDevice(device_id);
}

boost::optional<SubRegionAllocation> BodyGraph::findRegion(nsvr_bodypart bodypart, double r, double s)
{
	const auto& region = m_namedRegions[bodypart];
	SubRegionAllocation id;
	if (region.search(r, s, &id)) {
		return id;
	}
	
	return boost::none;
}

void BodyGraph::ClearAssociations(uint64_t device_id)
{
	BGL_FORALL_VERTICES_T(v, m_nodes, LabeledGraph) {
		m_nodes.graph()[v].removeDevice(device_id);
	}
}

/*

TO process a haptic request for a named, specific region
find the devices associated with the "best match" to that region
send the request to those devices

TO find the "best match" of a region
search the m_namedRegions hashtable for a NamedRegion where region.contains(subregion)
return that subregion

TO perform region.contains(subregion)
check if any child regions contain the region
if so, recurse

if contains(subregion)
return this subregion id;



*/

boost::optional<SubRegionAllocation> BodyGraph::GetBestMatchingSubregion(SubRegionAllocation id)
{
	for (const auto& thing : m_namedRegions) {
		SubRegionAllocation lowestValidAlloc;
		if (thing.second.contains_subregion(id, &lowestValidAlloc)) {
			return lowestValidAlloc;
		}
	}

	return boost::none;
}

std::vector<uint64_t> BodyGraph::GetDevicesForSubregion(SubRegionAllocation id)
{
	std::vector<std::pair<uint64_t, NodeData*>> candidates;
	BGL_FORALL_VERTICES_T(v, m_nodes, LabeledGraph) {
		uint64_t mag;
		if (NamedRegion::contains(m_nodes.graph()[v].mostSpecificRegion, id, &mag)) {
			candidates.emplace_back(mag, &m_nodes.graph()[v]);
		}
	}

	if (candidates.empty()) {
		std::vector<uint64_t>{};
	}
	auto result = std::max_element(candidates.begin(), candidates.end(), 
		[](const std::pair<uint64_t, NodeData*>& pair_a, const std::pair<uint64_t, NodeData*>& pair_b) {return pair_a.first < pair_b.first; });
	return result->second->m_assocDevices;
}

void BodyGraph::NodeData::addDevice(uint64_t id)
{
	auto it = std::find(m_assocDevices.begin(), m_assocDevices.end(), id);
	if (it == m_assocDevices.end()) {
		m_assocDevices.push_back(id);
	}
}

void BodyGraph::NodeData::removeDevice(uint64_t id)
{
	auto it = std::remove(m_assocDevices.begin(), m_assocDevices.end(), id);
	m_assocDevices.erase(it, m_assocDevices.end());
}
