#include "stdafx.h"
#include "BodyGraph.h"
#include <boost/graph/iteration_macros.hpp>

#define _USE_MATH_DEFINES
#include <math.h>


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


BodyGraph::BodyGraph()
{
	Bodypart upperArmLeft{ nsvr_bodypart_upperarm_left , 29.41 };
	subdivide(upperArmLeft, 10, 10);
	m_bodyparts[nsvr_bodypart_upperarm_left] = upperArmLeft;


	Bodypart lowerArmLeft{ nsvr_bodypart_forearm_left, 27.68 };
	subdivide(lowerArmLeft, 10, 10);
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
	subdivide(torso, 20, 10);
	m_bodyparts[nsvr_bodypart_torso] = torso;

	Bodypart head{ nsvr_bodypart_head, 19.03 };
	m_bodyparts[nsvr_bodypart_head] = head;

	Bodypart hips{ nsvr_bodypart_hips, 12.11 };
	m_bodyparts[nsvr_bodypart_hips] = hips;


	assert(NamedRegion::between_deg(0, 350 , 10));
	assert(NamedRegion::between_deg(359, 350, 10));
	assert(NamedRegion::between_deg(180, 170, 190));
	assert(NamedRegion::between_deg(180, 10, 270));
	assert(!NamedRegion::between_deg(0, 10, 270));
	assert(!NamedRegion::between_deg(5, 10, 270));
	assert(!NamedRegion::between_deg(280, 10, 270));
	assert(!NamedRegion::between_deg(359, 10, 270));
	assert(!NamedRegion::between_deg(360, 10, 270));

	m_namedRegions[nsvr_bodypart_torso].children = {
		NamedRegion(SubRegionId::nsvr_region_chest_front_left, nsvr_bodypart_torso, .75, 1.0, 350, 0),
		NamedRegion(SubRegionId::nsvr_region_ab_upper_left, nsvr_bodypart_torso, 0.50, 0.75, 350, 0),
		NamedRegion(SubRegionId::nsvr_region_ab_middle_left, nsvr_bodypart_torso, 0.25, 0.50, 350, 0),
		NamedRegion(SubRegionId::nsvr_region_ab_lower_left, nsvr_bodypart_torso, 0.0, 0.25, 350, 0),
		NamedRegion(SubRegionId::nsvr_region_torso_back_left, nsvr_bodypart_torso, 0.0, 1.0, 180, 270),

		NamedRegion(SubRegionId::nsvr_region_chest_front_right, nsvr_bodypart_torso, .75, 1.0, 0, 10),
		NamedRegion(SubRegionId::nsvr_region_ab_upper_right, nsvr_bodypart_torso, 0.50, 0.75, 0, 10),
		NamedRegion(SubRegionId::nsvr_region_ab_middle_right, nsvr_bodypart_torso, 0.25, 0.50, 0, 10),
		NamedRegion(SubRegionId::nsvr_region_ab_lower_right, nsvr_bodypart_torso, 0.0, 0.25, 0, 10),
		NamedRegion(SubRegionId::nsvr_region_torso_back_right, nsvr_bodypart_torso, 0.0, 1.0, 90, 180)
	};

	m_namedRegions[nsvr_bodypart_upperarm_left].children = {
		NamedRegion(SubRegionId::nsvr_region_shoulder_left, nsvr_bodypart_upperarm_left, 0.90, 1.0, 0, 360),
		NamedRegion(SubRegionId::nsvr_region_upper_arm_left, nsvr_bodypart_upperarm_left, 0.0, 0.90, 0, 360)
	};
	m_namedRegions[nsvr_bodypart_upperarm_right].children = {
		NamedRegion(SubRegionId::nsvr_region_shoulder_right, nsvr_bodypart_upperarm_right, 0.90, 1.0, 0, 360),
		NamedRegion(SubRegionId::nsvr_region_upper_arm_right, nsvr_bodypart_upperarm_right, 0.0, 0.90, 0, 360)
	};


	m_namedRegions[nsvr_bodypart_forearm_left].children = {
		NamedRegion(SubRegionId::nsvr_region_lower_arm_left, nsvr_bodypart_forearm_left, 0.0, 1.0, 0, 360)
	};

	m_namedRegions[nsvr_bodypart_forearm_right].children = {
		NamedRegion(SubRegionId::nsvr_region_lower_arm_right, nsvr_bodypart_forearm_right, 0.0, 1.0, 0, 360)
	};
}


int BodyGraph::CreateNode(const char * name, nsvr_bodygraph_region * pose)
{
	boost::add_vertex(name, m_nodes);
	NamedRegion id{ SubRegionId::nsvr_region_unknown, nsvr_bodypart_unknown, 0,0,0,0 };
	
	std::cout << "[" << name << "] Searching for bodypart=" << pose->bodypart << ", " << pose->rotation << "\n";

	if (auto region = findRegion(pose->bodypart, pose->segment_ratio, pose->rotation)) {
		//id = *region;
		std::cout << "	Found a valid region: " << (+*region)._to_string() << "\n";

	}
	else {
		std::cout << "	Didn't find any regions matching that.\n";
	}
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

boost::optional<SubRegionId> BodyGraph::findRegion(nsvr_bodypart bodypart, double r, double s)
{
	const auto& region = m_namedRegions[bodypart];
	SubRegionId id = SubRegionId::nsvr_region_unknown;
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
