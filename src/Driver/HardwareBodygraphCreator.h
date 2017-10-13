#pragma once
#include "BodyGraphCreator.h"
#include "BodyGraph.h"
#include <boost/optional.hpp>

class HardwareBodygraphCreator : public BodyGraphCreator {
public:
	HardwareBodygraphCreator(bodygraph_api* api);
	void provideDescription(Parsing::BodyGraphDescriptor descriptor);

	std::vector<nsvr_node_id> GetNodesAtRegion(nsvr_region region) const final;
	std::vector<nsvr_node_id> GetNodesAtRegions(const std::vector<nsvr_region>& region) const final;
	void ForEachNodeAtRegion(nsvr_region region, BodyGraphCreator::NodeAction action) final;
	void ForEachRegionPresent(std::function<void(nsvr_region, const std::vector<nsvr_node_id>&)> action) final;
	std::vector<nsvr_region> GetRegionsForNode(nsvr_node_id node) const final;
	void fetchDynamically();

private:
	boost::optional<Parsing::BodyGraphDescriptor> m_staticDescriptor;
	bodygraph_api* m_api;
	BodyGraph m_graph;
	void fetchFromDescription(const Parsing::BodyGraphDescriptor& descriptor);
};