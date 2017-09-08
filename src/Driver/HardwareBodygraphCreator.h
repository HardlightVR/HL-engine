#pragma once
#include "BodyGraphCreator.h"
#include "BodyGraph.h"


class HardwareBodygraphCreator : public BodyGraphCreator {
public:
	HardwareBodygraphCreator(const Parsing::BodyGraphDescriptor& descriptor, bodygraph_api* api);


	void ForEachNodeAtRegion(nsvr_region region, BodyGraphCreator::NodeAction action) override;
private:
	bodygraph_api* m_api;
	BodyGraph m_graph;
	void fetchFromDescription(const Parsing::BodyGraphDescriptor& descriptor);
	void fetchDynamically();
};