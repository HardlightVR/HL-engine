#include "stdafx.h"
#include "BodyGraph.h"
#include <boost/graph/iteration_macros.hpp>
BodyGraph::BodyGraph() 

{
	SubRegion upperArmLeft{ SubRegionId::nsvr_region_upper_arm_left };
	upperArmLeft.real_segment_length = 29.41;
	upperArmLeft.children = {
		SubRegion {SubRegionId::nsvr_region_shoulder_left, 0.8, 1.0, -360, 360, 5}
	};
	m_namedRegions[nsvr_bodypart_upperarm_left] = upperArmLeft;

	SubRegion lowerArmLeft{ SubRegionId::nsvr_region_lower_arm_right};
	lowerArmLeft.real_segment_length = 27.68;
	m_namedRegions[nsvr_bodypart_forearm_left] = lowerArmLeft;

	SubRegion upperLegLeft{ SubRegionId::nsvr_region_upper_leg_left };
	upperLegLeft.real_segment_length = 43.25;
	m_namedRegions[nsvr_bodypart_upperleg_left] = upperLegLeft;

	SubRegion lowerLegLeft{ SubRegionId::nsvr_region_lower_leg_left };
	lowerLegLeft.real_segment_length = 43.25;
	m_namedRegions[nsvr_bodypart_lowerleg_left] = lowerLegLeft;

	SubRegion torso{ SubRegionId::nsvr_region_torso };
	torso.real_segment_length = 51.9;
	torso.children = {
		SubRegion { SubRegionId::nsvr_region_chest_front_left, .75, 1.0, -75, 0 , 15},
		SubRegion { SubRegionId::nsvr_region_ab_upper_left, .55, .75, -75, 0,  12.3},
		SubRegion { SubRegionId::nsvr_region_ab_middle_left , .45, .55, -75, 0, 12.3},
		SubRegion { SubRegionId::nsvr_region_ab_lower_left, 0, .45, -75, 0, 12.3}
	};

	m_namedRegions[nsvr_bodypart_torso] = torso;
	
	SubRegion head{ SubRegionId::nsvr_region_head };
	head.real_segment_length = 19.03;
	m_namedRegions[nsvr_bodypart_head] = head;
	SubRegion hips{ SubRegionId::nsvr_region_gluteal };
	m_namedRegions[nsvr_bodypart_hips] = hips;
	hips.real_segment_length = 12.11;




}

int BodyGraph::CreateNode(const char * name, nsvr_bodygraph_region * pose)
{
	boost::add_vertex(name, m_nodes);
	SubRegionId id = SubRegionId::nsvr_region_unknown;
	
	std::cout << "Searching for bodypart=" << pose->bodypart << ", " << pose->rotation << "\n";

	if (m_namedRegions[pose->bodypart].search(pose->segment_ratio, pose->rotation, &id)) {
		std::cout << "	Found a valid region: " << (+id)._to_string() << "\n";

	}
	else {
		std::cout << "	Didn't find any regions matching that.\n";
	}
	m_nodes[name] = { name, *pose, id };




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
