#include "stdafx.h"
#include "BodyGraph.h"
#include <boost/graph/iteration_macros.hpp>
#include <memory>
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
	using region = subregion::shared_region;

	auto entire_torso = std::shared_ptr<subregion>(new 
		subregion(region::identifier_torso, segment_range::full, angle_range::full, {
			subregion(region::identifier_torso_front, segment_range::full, angle_range::front_half,{
				subregion(region::identifier_chest_left,  segment_range{ 0.8, 1.0 }, angle_range{ 340, 360 }),
				subregion(region::identifier_upper_ab_left,  segment_range{ 0.6, 0.8 }, angle_range{ 340, 360 }),
				subregion(region::identifier_middle_ab_left, segment_range{ 0.4, 0.6 }, angle_range{ 340, 360 }),
				subregion(region::identifier_lower_ab_left,  segment_range{ 0.0, 0.4 }, angle_range{ 340, 360 }),
				subregion(region::identifier_chest_right, segment_range{ 0.8, 1.0 }, angle_range{ 0, 20 }),
				subregion(region::identifier_upper_ab_right, segment_range{ 0.6, 0.8 }, angle_range{ 0, 20 }),
				subregion(region::identifier_middle_ab_right, segment_range{ 0.4, 0.6 }, angle_range{ 0, 20 }),
				subregion(region::identifier_lower_ab_right, segment_range{ 0.0, 0.4 }, angle_range{ 0, 20 })
			}),
			subregion(region::identifier_torso_back, segment_range::full, angle_range::back_half,{
				subregion(region::identifier_upper_back_left,  segment_range{ 0.5, 1.0 }, angle_range{ 180, 270 }),
				subregion(region::identifier_upper_back_right,  segment_range{ 0.5, 1.0 }, angle_range{ 90, 180 })
			}),
			subregion(region::identifier_torso_left, segment_range::full, angle_range::left_half),
			subregion(region::identifier_torso_right, segment_range::full, angle_range::right_half)
	}));


	auto upper_arm_left = std::shared_ptr<subregion>(new
		subregion(region::identifier_upper_arm_left, segment_range::full, angle_range::full, {
			subregion(region::identifier_shoulder_left, segment_range{ 0.85, 1.0 }, angle_range::full)
		}));


	auto upper_arm_right = std::shared_ptr<subregion>(new
		subregion(region::identifier_upper_arm_right, segment_range::full, angle_range::full, {
			subregion(region::identifier_shoulder_right, segment_range{ 0.85, 1.0 }, angle_range::full)
		}));



	auto lower_arm_left = std::shared_ptr<subregion>(new
		subregion(region::identifier_lower_arm_left, segment_range::full, angle_range::full));


	auto lower_arm_right = std::shared_ptr<subregion>(new
		subregion(region::identifier_lower_arm_right, segment_range::full, angle_range::full));
	

	auto upper_leg_left = std::shared_ptr<subregion>(new
		subregion(region::identifier_upper_leg_left, segment_range::full, angle_range::full));

	auto lower_leg_left = std::shared_ptr<subregion>(new
		subregion(region::identifier_lower_leg_left, segment_range::full, angle_range::full));

	auto upper_leg_right = std::shared_ptr<subregion>(new
		subregion(region::identifier_upper_leg_right, segment_range::full, angle_range::full));

	auto lower_leg_right = std::shared_ptr<subregion>(new
		subregion(region::identifier_lower_leg_right, segment_range::full, angle_range::full));

	auto head = std::shared_ptr<subregion>(new
		subregion(region::identifier_head, segment_range::full, angle_range::full));
	
	auto left_palm = std::shared_ptr<subregion>(new
		subregion(region::identifier_palm_left, segment_range::full, angle_range::full));

	auto right_palm = std::shared_ptr<subregion>(new
		subregion(region::identifier_palm_right, segment_range::full, angle_range::full));

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


	m_bodyparts.emplace(nsvr_bodypart_palm_left, Bodypart(nsvr_bodypart_palm_left, 10.0, left_palm));
	m_bodyparts.emplace(nsvr_bodypart_palm_right, Bodypart(nsvr_bodypart_palm_right, 10.0, right_palm));

	for (auto& bp : m_bodyparts) {
		bp.second.region->init_backlinks();
	}

}

subregion::subregion()
	: region(shared_region::identifier_unknown)
	, seg{ 0, 0 }
	, ang{ 0,0 }
	, coords{ 0, 0, 0 }
	, children()
	, hardware_defined_regions()
	, parent(nullptr) {}

subregion::subregion(shared_region region, segment_range segment_offset, angle_range angle_range)
	: region(region)
	, seg(segment_offset)
	, ang(angle_range)
	, children()
	, hardware_defined_regions()
	, parent(nullptr) {
	calculateCoordinates();

}

subregion::subregion(shared_region region, segment_range segment_offset, angle_range angle_range, std::vector<subregion> child_regions)
	: region(region)
	, seg(segment_offset)
	, ang(angle_range)
	, children(std::move(child_regions))
	, hardware_defined_regions()
	, parent(nullptr) {

	calculateCoordinates();


}
int BodyGraph::CreateNode(const char * name, nsvr_bodygraph_region * pose)
{
	//To create a new node:
		//First, add a vertex into the graph with the given name.
		boost::add_vertex(name, m_nodes);

		//Then, grab the bodypart corresponding with the given arguments.
		auto& bodypart = m_bodyparts[pose->bodypart];

		//Then ask that bodypart to find the nearest match to the given coordinates.
		subregion::shared_region region = bodypart.region->find_best_match(pose->segment_ratio, pose->rotation).second;

		//Update the vertex with the information that was found
		m_nodes[name] = NodeData(name, *pose, region);

		//Update the bodypart's subregion that corresponded with the match to have a reference to this graph node.
		bodypart.region->find(region)->hardware_defined_regions.push_back(name);



	std::cout << "[" << name << "] Registered on region " << (uint64_t)region << "\n";
	return 0;
}


int BodyGraph::ConnectNodes(const char* a, const char* b)
{
	boost::add_edge_by_label(a, b, m_nodes);
	return 0;
}

void BodyGraph::Associate(const char * node, uint64_t device_id)
{
	m_nodes[node].addNode(device_id);
}

void BodyGraph::Unassociate(const char * node, uint64_t device_id)
{
	m_nodes[node].removeNode(device_id);
}



void BodyGraph::ClearAssociations(uint64_t device_id)
{
	BGL_FORALL_VERTICES_T(v, m_nodes, LabeledGraph) {
		m_nodes.graph()[v].removeNode(device_id);
	}
}


// Retrieve any nodes that are associated with a particular named region
std::vector<nsvr_node_id> BodyGraph::getNodesForNamedRegion(subregion::shared_region region)
{
	std::vector<nsvr_node_id> nodes;

	for (auto& bp : m_bodyparts) {
		subregion* ptr = bp.second.region->find(region);
		if (ptr != nullptr) {
			
			//If this region doesn't have any devices associated with it,
			//Then we need to traverse upwards until we find one that does!
			while (ptr->hardware_defined_regions.empty()) {
				if (ptr->parent != nullptr) {
					ptr = ptr->parent;
				}
				else {
					break;
				}
			}
			
			//Okay, we found some references - for each one, grab the associated nodes out of the graph
			//and insert into our list.
			for (const std::string& name : ptr->hardware_defined_regions) {
				auto& found = m_nodes[name].nodes;
				nodes.insert(nodes.end(), found.begin(), found.end());
			}
		}
	}

	std::sort(nodes.begin(), nodes.end());
	nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
	return nodes;
	
}


std::unordered_map<subregion::shared_region, std::vector<nsvr_node_id>> BodyGraph::getAllNodes() const
{

	std::unordered_map<subregion::shared_region, std::vector<nsvr_node_id>> nodes;

	//Walk through the graph, and add any nodes that were found to the hashtable
	BGL_FORALL_VERTICES_T(v, m_nodes, LabeledGraph) {
		subregion::shared_region region = m_nodes.graph()[v].computed_region;
		auto& nodeList = m_nodes.graph()[v].nodes;
		auto& currentDevices = nodes[region];
		currentDevices.insert(currentDevices.end(), nodeList.begin(), nodeList.end());
	}

	//make sure each list has no duplicates
	for (auto& kvp : nodes) {
		auto& list = kvp.second;
		std::sort(list.begin(), list.end());
		list.erase(std::unique(list.begin(), list.end()), list.end());
	}

	return nodes;
}



void BodyGraph::NodeData::addNode(nsvr_node_id id)
{
	auto it = std::find(nodes.begin(), nodes.end(), id);
	if (it == nodes.end()) {
		nodes.push_back(id);
	}
}

void BodyGraph::NodeData::removeNode(nsvr_node_id id)
{
	auto it = std::remove(nodes.begin(), nodes.end(), id);
	nodes.erase(it, nodes.end());
}


double distance(double x, double y, double z, double x2, double y2, double z2) {
	return sqrt(pow((x - x2), 2) + pow((y - y2), 2) + pow((z - z2), 2));
}

