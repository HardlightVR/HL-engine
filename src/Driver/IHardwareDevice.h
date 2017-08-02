#pragma once

#include <memory>
#include "DriverConfigParser.h"
#include "cevent_internal.h"
#include <vector>

#include "protobuff_defs/HighLevelEvent.pb.h"
#include "pevent.h"

#include "PluginApis.h"
#include <boost/variant.hpp>
#include <boost/signals2.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>

class PluginApis;
class PluginEventHandler;
class HardwareCoordinator;
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
	HapticNode(const NodeDescriptor& info, PluginApis*);

	void deliver(RequestId, const nsvr::cevents::request_base&) override;
	std::string getRegion() const override;
private:
	PluginApis* m_apis;
	std::string m_region;
};


class TrackingNode : public Node {
public:
	TrackingNode(const NodeDescriptor& info, PluginApis*);
	void deliver(RequestId, const nsvr::cevents::request_base&) override;
	std::string getRegion() const override;

	void BeginTracking();
	void EndTracking();
	void DeliverTracking(nsvr_quaternion* quat);

	boost::signals2::signal<void(const char*, nsvr_quaternion*)> TrackingSignal;

private:
	PluginApis* m_apis;
	std::string m_region;
	nsvr_quaternion m_latestQuat;
};



class NodalDevice {
public:
	using Region = std::string;
	using RequestId = uint64_t;
	NodalDevice(const HardwareDescriptor& desc, PluginApis& api, PluginEventHandler& ev);

	NodalDevice& operator=(const NodalDevice&) = delete;
	NodalDevice(const NodalDevice&) = delete;
	void deliverRequest(const NullSpaceIPC::HighLevelEvent& event);
	
	std::string name() const;
	bool hasCapability(Apis name) const;

	void setupHooks(HardwareCoordinator& coordinator);
	void teardownHooks();

	
private:
	
	HardwareDescriptor::Concept m_concept;
	std::string m_name;

	std::vector<std::unique_ptr<HapticNode>> m_hapticDevices;
	std::vector<std::unique_ptr<TrackingNode>> m_trackingDevices;
	std::unordered_map<Region, std::vector<Node*>> m_nodesByRegion;
	PluginApis* m_apis;

	void figureOutCapabilities();
	void setupSubscriptions(PluginEventHandler& ev);
	void parseDevices(const std::vector<NodeDescriptor>& descriptor);
	void fetchDynamicDevices();

	void handle_connect(const nsvr::pevents::connected_event&);
	void handle_disconnect(const nsvr::pevents::disconnected_event&);

	void handleSimpleHaptic(RequestId id, const ::NullSpaceIPC::SimpleHaptic& simple);
	void handlePlaybackEvent(RequestId id, const ::NullSpaceIPC::PlaybackEvent& event);
	


};



