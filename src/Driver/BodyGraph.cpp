#include "stdafx.h"
#include "BodyGraph.h"

BodyGraph::BodyGraph() 

{
	SubRegion upperArmLeft{ SubRegionId::nsvr_region_upper_arm_left };
	upperArmLeft.children = {
		SubRegion {SubRegionId::nsvr_region_shoulder_left, 0.8, 1.0, -360, 360}
	};

	m_namedRegions[nsvr_bodypart_upperarm_left] = upperArmLeft;

	SubRegion lowerArmLeft{ SubRegionId::nsvr_region_lower_arm_right};

	m_namedRegions[nsvr_bodypart_forearm_left] = lowerArmLeft;

	SubRegion upperLegLeft{ SubRegionId::nsvr_region_upper_leg_left };

	m_namedRegions[nsvr_bodypart_upperleg_left] = upperLegLeft;

	SubRegion lowerLegLeft{ SubRegionId::nsvr_region_lower_leg_left };

	m_namedRegions[nsvr_bodypart_lowerleg_left] = lowerLegLeft;

	SubRegion torso{ SubRegionId::nsvr_region_torso };
	torso.children = {
		SubRegion { SubRegionId::nsvr_region_chest_front_left, .75, 1.0, -75, 0},
		SubRegion { SubRegionId::nsvr_region_ab_upper_left, .55, .75, -75, 0 },
		SubRegion { SubRegionId::nsvr_region_ab_middle_left , .45, .55, -75, 0},
		SubRegion { SubRegionId::nsvr_region_ab_lower_left, 0, .45, -75, 0}
	};

	m_namedRegions[nsvr_bodypart_torso] = torso;
	
	SubRegion head{ SubRegionId::nsvr_region_head };

	m_namedRegions[nsvr_bodypart_head] = head;
	SubRegion hips{ SubRegionId::nsvr_region_gluteal };
	m_namedRegions[nsvr_bodypart_hips] = hips;




}

int BodyGraph::CreateNode(const char * name, nsvr_bodygraph_region * pose)
{
	boost::add_vertex(name, m_nodes);
	SubRegionId id = SubRegionId::nsvr_region_unknown;
	if (m_namedRegions[pose->parallel.bodypart].search(pose->parallel.parallel, pose->rotation, &id)) {
		std::cout << "Found";
	}



	m_nodes[name] = {name, *pose};

	return 0;
}

int BodyGraph::CreateNodeRelative(const char * a, nsvr_region_relation relation, const char * B, double offset)
{
	boost::add_vertex(a, m_nodes);
	nsvr_bodygraph_region region = m_nodes[B].region;
	
	if (relation == nsvr_region_relation_below) {
		region.parallel.parallel -= 0.1; //cm
	}
	else if (relation == nsvr_region_relation_above) {
		region.parallel.parallel += 0.1; //cm
	}
	else if (relation == nsvr_region_relation_left) {
		region.rotation -= 5; //degrees
	}
	else if (relation == nsvr_region_relation_right) {
		region.rotation += 5; //degrees
	}

	m_nodes[a] = { a, region };

	
	return 0;
}

int BodyGraph::ConnectNodes(const char* a, const char* b)
{
	boost::add_edge_by_label(a, b, m_nodes);
	return 0;
}
