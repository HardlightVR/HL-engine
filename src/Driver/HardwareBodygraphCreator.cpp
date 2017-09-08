#include "stdafx.h"
#include "HardwareBodygraphCreator.h"



class region_visitor : public boost::static_visitor<void> {
private:
	BodyGraph& m_graph;
public:
	region_visitor(BodyGraph& graph) : m_graph(graph) {

	}
	void operator()(const Parsing::SingleRegionDescriptor& single) {
		nsvr_bodygraph_region region;
		region.bodypart = single.bodypart;
		region.rotation = single.location.rotation;
		region.segment_ratio = single.location.height;
		m_graph.CreateNode(single.name.c_str(), &region);
	}


	void operator()(const Parsing::MultiRegionDescriptor& multi) {
		for (std::size_t i = 0; i < multi.count; i++) {
			auto interp = Parsing::lerp(multi.location_start, multi.location_end, (float)i / (multi.count - 1));
			nsvr_bodygraph_region region;
			region.bodypart = multi.bodypart;
			region.rotation = interp.rotation;
			region.segment_ratio = interp.height;
			m_graph.CreateNode(std::string(multi.name + ":" + std::to_string(i)).c_str(), &region);
		}
	}
};


HardwareBodygraphCreator::HardwareBodygraphCreator(const Parsing::BodyGraphDescriptor& descriptor, bodygraph_api* api)
	: m_api(api)
	, m_graph()
{
	fetchFromDescription(descriptor);
	fetchDynamically();
}

std::vector<nsvr_node_id> HardwareBodygraphCreator::GetNodesAtRegion(nsvr_region region) const
{
	return m_graph.getNodesForNamedRegion(static_cast<subregion::shared_region>(region));

}

void HardwareBodygraphCreator::fetchFromDescription(const Parsing::BodyGraphDescriptor & descriptor)
{
	region_visitor visitor(m_graph);
	for (const auto& region : descriptor.regions) {
		boost::apply_visitor(visitor, region);
	}
}

void HardwareBodygraphCreator::fetchDynamically()
{
	m_api->submit_setup(reinterpret_cast<nsvr_bodygraph*>(&m_graph));
}


void HardwareBodygraphCreator::ForEachNodeAtRegion(nsvr_region region, BodyGraphCreator::NodeAction action)
{
	std::vector<nsvr_node_id> node_ids = m_graph.getNodesForNamedRegion(static_cast<subregion::shared_region>(region));

	std::for_each(node_ids.begin(), node_ids.end(), action);
}


