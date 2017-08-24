#include "stdafx.h"
#include "BodyGraph.h"
#include <boost/graph/iteration_macros.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

double to_radians(double degrees) {
	return (degrees * M_PI) / 180.0;
}
double to_degrees(double radians)
{
	return (radians * 180.0) / M_PI;
}
const segment_range segment_range::full = { 0, 1 };
const segment_range segment_range::lower_half = { 0, 0.5 };
const segment_range segment_range::upper_half = { 0.5, 1.0 };

const angle_range angle_range::full = { 0, 360 };
const angle_range angle_range::left_half = { 180, 360 };
const angle_range angle_range::right_half = { 0, 180 };
const angle_range angle_range::front_half = { 270, 90 };
const angle_range angle_range::back_half = { 90, 270 };

BodyGraph::BodyGraph()
{


	auto entire_torso = 
	subregion(identifier_torso, segment_range::full, angle_range::full, {
		subregion(identifier_torso_front, segment_range::full, angle_range::front_half, {
			subregion(identifier_chest_left,  segment_range{0.8, 1.0}, angle_range{340, 360}),
			subregion(identifier_upper_ab_left,  segment_range{ 0.6, 0.8 }, angle_range{ 340, 360 }),
			subregion(identifier_middle_ab_left, segment_range{0.4, 0.6}, angle_range{340, 360}),
			subregion(identifier_lower_ab_left,  segment_range{0.0, 0.4}, angle_range{340, 360}),
			subregion(identifier_chest_right, segment_range{ 0.8, 1.0 }, angle_range{ 0, 20 }),
			subregion(identifier_upper_ab_right, segment_range{ 0.6, 0.8 }, angle_range{ 0, 20 }),
			subregion(identifier_middle_ab_right, segment_range{ 0.4, 0.6 }, angle_range{ 0, 20 }),
			subregion(identifier_lower_ab_right, segment_range{ 0.0, 0.4 }, angle_range{ 0, 20 })
		}),
		subregion(identifier_torso_back, segment_range::full, angle_range::back_half, {
			subregion(identifier_upper_back_left,  segment_range{ 0.5, 1.0 }, angle_range{ 180, 270 }),
			subregion(identifier_upper_back_right,  segment_range{ 0.5, 1.0 }, angle_range{ 90, 180 })
		}),
		subregion(identifier_torso_left, segment_range::full, angle_range::left_half),
		subregion(identifier_torso_right, segment_range::full, angle_range::right_half)
	});



	auto upper_arm_left =
		subregion(identifier_upper_arm_left, segment_range::full, angle_range::full, {
			subregion(identifier_shoulder_left, segment_range{0.8, 1.0}, angle_range::full)
		});
	auto upper_arm_right =
		subregion(identifier_upper_arm_right, segment_range::full, angle_range::full, {
			subregion(identifier_shoulder_right, segment_range::full, angle_range:: full)
		});

	auto lower_arm_left =
		subregion(identifier_lower_arm_left, segment_range::full, angle_range::full);

	auto lower_arm_right =
		subregion(identifier_lower_arm_right, segment_range::full, angle_range::full);
	
	auto upper_leg_left =
		subregion(identifier_upper_leg_left, segment_range::full, angle_range::full);

	auto lower_leg_left =
		subregion(identifier_lower_leg_left, segment_range::full, angle_range::full);
	
	auto upper_leg_right =
		subregion(identifier_upper_leg_right, segment_range::full, angle_range::full);

	auto lower_leg_right =
		subregion(identifier_lower_leg_right, segment_range::full, angle_range::full);
	
	auto head =
		subregion(identifier_head, segment_range::full, angle_range::full);
	
	m_bodyparts.emplace(nsvr_bodypart_head, Bodypart(nsvr_bodypart_head, 19.03, head));

	m_bodyparts.emplace(nsvr_bodypart_torso, Bodypart(nsvr_bodypart_torso, 51.9, entire_torso));

	m_bodyparts.emplace(nsvr_bodypart_upperarm_left, Bodypart( nsvr_bodypart_upperarm_left , 29.41, upper_arm_left ));

	m_bodyparts.emplace(nsvr_bodypart_forearm_left, Bodypart ( nsvr_bodypart_forearm_left, 27.68, lower_arm_left ));

	m_bodyparts.emplace(nsvr_bodypart_upperarm_right, Bodypart( nsvr_bodypart_upperarm_right, 29.41, upper_arm_right ));

	m_bodyparts.emplace(nsvr_bodypart_forearm_right, Bodypart(nsvr_bodypart_forearm_right, 27.68, lower_arm_right));

	m_bodyparts.emplace(nsvr_bodypart_forearm_right, Bodypart(nsvr_bodypart_forearm_right, 27.68, lower_arm_right));

	m_bodyparts.emplace(nsvr_bodypart_lowerleg_left, Bodypart(nsvr_bodypart_lowerleg_left, 43.25, lower_leg_left));

	m_bodyparts.emplace(nsvr_bodypart_upperleg_left, Bodypart(nsvr_bodypart_upperleg_left, 43.25, upper_leg_left));

	m_bodyparts.emplace(nsvr_bodypart_lowerleg_right, Bodypart(nsvr_bodypart_lowerleg_right, 43.25, lower_leg_right));

	m_bodyparts.emplace(nsvr_bodypart_upperleg_right, Bodypart(nsvr_bodypart_upperleg_right, 43.25, upper_leg_right));

}



int BodyGraph::CreateNode(const char * name, nsvr_bodygraph_region * pose)
{
	boost::add_vertex(name, m_nodes);
	SubRegionAllocation id = SubRegionAllocation::reserved_block_1;
	
	std::cout << "[" << name << "] Searching for bodypart=" << pose->bodypart << ", " << pose->rotation << "\n";

	//if (auto region = findRegion(pose->bodypart, pose->segment_ratio, pose->rotation)) {
		//id = *region;
		//std::cout << "	Found a valid region: " << (+*region)._to_string() << "\n";
		//id = *region;
		//todo: fix for subregion alloc

	//}
	//else {
		std::cout << "	Didn't find any regions matching that.\n";
	//}
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


double distance(double x, double y, double z, double x2, double y2, double z2) {
	return sqrt(pow((x - x2), 2) + pow((y - y2), 2) + pow((z - z2), 2));
}

