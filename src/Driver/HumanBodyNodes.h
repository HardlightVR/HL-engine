#pragma once

/// A node-representation of the human body
/// for use in displaying what hardware is in use
/// within a game.


#include <unordered_map>
#include <vector>

#include "IHardwareDevice.h"

#include "NodeView.h"

#include "Renderable.h"

class HumanBodyNodes {

public:
	enum class NamedRegion {
		Unknown = 0,
		Unclassified = 1,
		Chest_Top_Left,
		Chest_Mid_Left,
		Chest_Bot_Left,
		Shoulder_Left,
		Back_Upper_Left,
		Back_Lower_Left,
		Arm_Upper_Left,
		Arm_Lower_Left,
		Hand_Left,
		Chest_Top_Right,
		Chest_Mid_Right,
		Chest_Bot_Right,
		Shoulder_Right,
		Back_Upper_Right,
		Back_Lower_Right,
		Arm_Upper_Right,
		Arm_Lower_Right,
		Hand_Right
	};	
	void AddNode(NamedRegion where, Renderable* node);
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

	std::unordered_map<NamedRegion, NodeInfo> m_info;

};
