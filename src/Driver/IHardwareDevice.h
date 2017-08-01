#pragma once

#include <memory>
#include "DriverConfigParser.h"
#include "cevent_internal.h"
#include <vector>

#include "protobuff_defs/HighLevelEvent.pb.h"
#include "pevent.h"

class PluginApis;
class PluginEventHandler;

class Node {
public:
	using RequestId = uint64_t;
	Node(uint64_t id, const std::string& name, uint32_t capability);

	virtual ~Node() {}
	virtual void deliver(RequestId id, const nsvr::cevents::request_base&) = 0;
	virtual std::string getRegion() const = 0;
	
	uint64_t id() const;
	std::string name() const;
protected:
	std::string m_name;
	uint64_t m_id;
	uint32_t m_capability;
};

class HapticNode : public Node {
public:
	HapticNode(const NodeDescriptor& info, PluginApis&);

	void deliver(RequestId, const nsvr::cevents::request_base&) override;
	std::string getRegion() const override;
private:
	PluginApis& m_apis;
	std::string m_region;
};

class TrackingNode : public Node {
public:
	TrackingNode(const NodeDescriptor& info, PluginApis&);
	void deliver(RequestId, const nsvr::cevents::request_base&) override;
	std::string getRegion() const override;
private:
	PluginApis& m_apis;
	std::string m_region;
	
};

class NodalDevice {
public:
	using Region = std::string;
	using RequestId = uint64_t;
	NodalDevice(HardwareDescriptor& desc, PluginApis& api, PluginEventHandler& ev);
	virtual ~NodalDevice() {}

	void deliverRequest(const NullSpaceIPC::HighLevelEvent& event);
	void addNode(std::unique_ptr<Node>);
	
	std::string name() const;

private:
	std::string m_name;
	std::vector<std::unique_ptr<Node>> m_nodes;
	std::unordered_map<Region, std::vector<Node*>> m_nodesByRegion;
	PluginApis& m_apis;


	void setupSubscriptions(PluginEventHandler& ev);
	void parseDevices(const std::vector<NodeDescriptor>& descriptor);
	void fetchDynamicDevices();

	void handle_connect(const nsvr::pevents::connected_event&);
	void handle_disconnect(const nsvr::pevents::disconnected_event&);

	void handleSimpleHaptic(RequestId id, const ::NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(RequestId id, const ::NullSpaceIPC::PlaybackEvent& event);
	
	Node* findDevice(uint64_t id);


};


class SuitDevice : public NodalDevice {
public:
	SuitDevice(HardwareDescriptor desc, PluginApis& cap, PluginEventHandler& ev);
private:
	void handle_tracking(const nsvr::pevents::tracking_event&);
};

class ControllerDevice : public NodalDevice {
public:
	ControllerDevice(HardwareDescriptor desc, PluginApis& cap, PluginEventHandler& ev);
};


namespace device_factories {

	std::unique_ptr<NodalDevice> createDevice(const HardwareDescriptor& description, PluginApis& cap, PluginEventHandler& ev);

}



