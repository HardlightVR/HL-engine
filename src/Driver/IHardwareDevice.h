#pragma once

#include <memory>
#include "DriverConfigParser.h"
#include <vector>

#include "protobuff_defs/HighLevelEvent.pb.h"
#include "pevent.h"

#include "PluginApis.h"
#include <boost/variant.hpp>
#include <boost/signals2.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include "BodyGraph.h"
#include "Renderable.h"
class PluginApis;
class PluginEventSource;
class HardwareCoordinator;

struct DeviceDescriptor;

struct NodeDescriptor {
	nsvr_node_type type;
	uint32_t capabilities;
	std::string displayName;
	nsvr_node_id id;
};
class Node {
public:
	Node(const NodeDescriptor&, PluginApis*);
	virtual ~Node() {}
	uint64_t id() const;
	std::string name() const;
protected:
	std::string m_name;
	uint64_t m_id;
	PluginApis* m_apis;
};

class HapticNode : public Node, public Renderable  {
public:
	HapticNode(const NodeDescriptor& info, PluginApis*);
	// Renderable support
	NodeView::Data Render() const override;
	NodeView::NodeType Type() const override;
	uint64_t Id() const override;

};


class TrackingNode : public Node {
public:
	TrackingNode(const NodeDescriptor& info, PluginApis*);
	void BeginTracking();
	void EndTracking();
	void DeliverTracking(nsvr_quaternion* quat);
	boost::signals2::signal<void(uint64_t, nsvr_quaternion*)> TrackingSignal;
private:
	nsvr_quaternion m_latestQuat;
};


class HumanBodyNodes;

class Device {
public:
	using RequestId = uint64_t;

	Device(const DeviceDescriptor& desc, PluginApis& api, PluginEventSource& ev);
	Device& operator=(const Device&) = delete;
	Device(const Device&) = delete;

	std::string name() const;

	void deliverRequest(const NullSpaceIPC::HighLevelEvent& event);
	

	void setupHooks(HardwareCoordinator& coordinator);
	void teardownHooks();

	void setupBodyRepresentation(HumanBodyNodes&);
	void teardownBodyRepresentation(HumanBodyNodes&);
	
	std::vector<NodeView> renderDevices() const;

	

	nsvr_device_id id() const;
private:
	
	std::string m_name;

	std::vector<std::unique_ptr<HapticNode>> m_hapticNodes;
	std::vector<std::unique_ptr<TrackingNode>> m_trackingNodes;
	PluginApis* m_apis;
	
	


	void dynamicallyFetchNodes();
	void parseNodes(const std::vector<NodeDescriptor>& descriptor);
	void createNewNode(const NodeDescriptor& descriptor);
	void fetchNodeInfo(uint64_t device_id);


	void handleSimpleHaptic(RequestId id, const ::NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(RequestId id, const ::NullSpaceIPC::PlaybackEvent& event);
	
	const Node* findNode(uint64_t id) const;
	Node* findNode(uint64_t id);

	BodyGraph m_graph;

	nsvr_device_id m_deviceId;
	std::atomic<bool> m_isBodyGraphSetup;

	 


};



