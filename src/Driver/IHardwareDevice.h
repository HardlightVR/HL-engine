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

class Node {
public:
	Node(const NodeDescriptor&);
	virtual ~Node() {}

	using RequestId = uint64_t;
	
	nsvr_region region() const;
	uint64_t id() const;
	std::string name() const;
protected:
	std::string m_name;
	uint64_t m_id;
	uint32_t m_capability;
	nsvr_region m_region;
};

class HapticNode : public Node, public Renderable  {
public:
	HapticNode(const NodeDescriptor& info, PluginApis*);


	// Renderable support
	NodeView::Data Render() const override;
	NodeView::NodeType Type() const override;
	uint64_t Id() const override;
private:
	PluginApis* m_apis;
};


class TrackingNode : public Node {
public:
	TrackingNode(const NodeDescriptor& info, PluginApis*);

	void BeginTracking();
	void EndTracking();
	void DeliverTracking(nsvr_quaternion* quat);

	boost::signals2::signal<void(nsvr_region, nsvr_quaternion*)> TrackingSignal;

private:
	PluginApis* m_apis;
	nsvr_quaternion m_latestQuat;
};


class HumanBodyNodes;
class NodalDevice {
public:
	using Region = std::string;
	using RequestId = uint64_t;
	NodalDevice(const HardwareDescriptor& desc, PluginApis& api, PluginEventSource& ev);

	NodalDevice& operator=(const NodalDevice&) = delete;
	NodalDevice(const NodalDevice&) = delete;
	void deliverRequest(const NullSpaceIPC::HighLevelEvent& event);
	
	std::string name() const;
	bool hasCapability(Apis name) const;

	void setupHooks(HardwareCoordinator& coordinator);
	void teardownHooks();

	void setupBodyRepresentation(HumanBodyNodes&);
	void teardownBodyRepresentation(HumanBodyNodes&);
	
	std::vector<NodeView> renderDevices();

	void run_update_loop_once(uint64_t dt);
private:
	
	HardwareDescriptor::Concept m_concept;
	std::string m_name;

	std::vector<std::unique_ptr<HapticNode>> m_hapticDevices;
	std::vector<std::unique_ptr<TrackingNode>> m_trackingDevices;
	std::vector<uint64_t> m_knownIds;
	PluginApis* m_apis;
	void fetchDeviceInfo(uint64_t device_id);

	void setupSubscriptions(PluginEventSource& ev);

	void createNewDevice(const NodeDescriptor& descriptor);
	void parseDevices(const std::vector<NodeDescriptor>& descriptor);
	void dynamicallyFetchDevices();

	void handle_connect(uint64_t device_id);
	void handle_disconnect(uint64_t device_id);

	void handleSimpleHaptic(RequestId id, const ::NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(RequestId id, const ::NullSpaceIPC::PlaybackEvent& event);
	
	Node* findDevice(uint64_t id);
	BodyGraph m_graph;

	std::atomic<bool> m_isBodyGraphSetup;

	 


};



