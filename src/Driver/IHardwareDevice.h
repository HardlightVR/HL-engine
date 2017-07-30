#pragma once

#include<memory>
#include "DriverConfigParser.h"
#include "cevent_internal.h"
#include <vector>

#include "protobuff_defs/HighLevelEvent.pb.h"


class PluginApis;
class PluginEventHandler;

class Node {
public:
	using RequestId = uint64_t;
	virtual ~Node() {}
	Node(uint64_t id, const std::string& name, uint32_t capability);
	virtual void deliver(RequestId id, const nsvr::cevents::request_base&) = 0;
	virtual std::string getRegion() const = 0;
	uint64_t id() const;
	std::string name() const;
protected:
	std::string m_name;
	uint64_t m_id;
	uint32_t m_capability;
};


class NodalDevice {
public:
	using Region = std::string;
	using RequestId = uint64_t;
	NodalDevice(HardwareDescriptor& desc, PluginApis& api, PluginEventHandler& ev);
	void deliverRequest(const NullSpaceIPC::HighLevelEvent& event);
	void addNode(std::unique_ptr<Node>);
	std::string name() const;
	virtual ~NodalDevice() {}

private:
	std::string m_name;
	std::vector<std::unique_ptr<Node>> m_nodes;
	Node* findDevice(uint64_t id);
	std::unordered_map<Region, std::vector<Node*>> m_nodesByRegion;
	void handleSimpleHaptic(uint64_t id, const ::NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(uint64_t id, const ::NullSpaceIPC::PlaybackEvent& event);
	PluginApis& m_apis;
};


class SuitDevice : public NodalDevice {
public:
	SuitDevice(HardwareDescriptor desc, PluginApis& cap, PluginEventHandler& ev);
};

class ControllerDevice : public NodalDevice {
public:
	ControllerDevice(HardwareDescriptor desc, PluginApis& cap, PluginEventHandler& ev);
};

class HapticNode : public Node {
public:
	HapticNode(const NodeDescriptor& info, PluginApis&, PluginEventHandler&);
	void deliver(RequestId, const nsvr::cevents::request_base&);
	std::string getRegion() const override;
private:
	PluginApis& m_apis;
	std::string m_region;

};





namespace device_factories {

	std::unique_ptr<NodalDevice> createDevice(const HardwareDescriptor& description, PluginApis& cap, PluginEventHandler& ev);
	//std::unique_ptr<NodalDevice> createDevice(const HardwareDescriptor& description, PluginApis&, PluginEventHandler&);

}



