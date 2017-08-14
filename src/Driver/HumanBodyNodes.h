#pragma once

/// A node-representation of the human body
/// for use in displaying what hardware is in use
/// within a game.


#include <unordered_map>
#include <vector>
#include "PluginAPI.h"
#include "IHardwareDevice.h"

#include "NodeView.h"

#include "Renderable.h"

class HumanBodyNodes {

public:
	
	void AddNode(nsvr_region where, Renderable* node);
	void RemoveNode(Renderable* node);

	std::vector<NodeView> GetNodeView();
private:

	class NodeInfo {
	public:
		void BeginMonitoring(Renderable* node);
		void EndMonitoring(Renderable* node);
		void Render(std::vector<NodeView::SingleNode>& view);
	private:
		std::vector<Renderable*> m_nodes;
	};

	std::unordered_map<nsvr_region, NodeInfo> m_info;

};
