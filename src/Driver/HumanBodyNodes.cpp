#include "stdafx.h"
#include "HumanBodyNodes.h"
#include <experimental\vector>
void HumanBodyNodes::AddNode(nsvr_region where, Renderable * node)
{
	m_info[where].BeginMonitoring(node);
}

void HumanBodyNodes::RemoveNode(Renderable * node)
{
	for (auto& info : m_info) {
		info.second.EndMonitoring(node);
	}
}

std::vector<NodeView> HumanBodyNodes::GetNodeView()
{
	std::vector<NodeView> view;
	for (auto& info : m_info) {
		NodeView nv;
		nv.region = static_cast<uint64_t>(info.first);
		info.second.Render(nv.nodes);
		view.push_back(std::move(nv));
	}

	return view;
}

void HumanBodyNodes::NodeInfo::BeginMonitoring(Renderable * node)
{
	m_nodes.push_back(node);
}

void HumanBodyNodes::NodeInfo::EndMonitoring(Renderable * node)
{
	std::experimental::erase_if(m_nodes, [node = node](Renderable* nodePtr) { return nodePtr == node; });
}

void HumanBodyNodes::NodeInfo::Render(std::vector<NodeView::SingleNode>& view)
{
	for (auto& node : m_nodes) {
		
		view.emplace_back(node->Type(), node->Render());
	}
}
