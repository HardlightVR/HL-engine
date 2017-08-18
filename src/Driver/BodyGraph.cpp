#include "stdafx.h"
#include "BodyGraph.h"

int BodyGraph::CreateNode(const char * name, nsvr_bodygraph_region * pose)
{
	boost::add_vertex(name, m_nodes);
	
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
