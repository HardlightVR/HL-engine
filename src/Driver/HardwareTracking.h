#pragma once

#include "PluginApis.h"
#include "DeviceIds.h"

class HardwareTracking {

public:



	template<typename Tag, typename Val>
	struct tag {
		Val value;
	};
	struct compass {};
	struct gravity {};
	using compass_val = tag<compass, nsvr_vector3>;
	using gravity_val = tag<gravity, nsvr_vector3>;
	using quaternion_val = nsvr_quaternion;
	using tracking_value = boost::variant<nsvr_quaternion, compass_val, gravity_val>;

	struct stream {
		std::function<void(tracking_value)> deliver;
	
	};

	HardwareTracking(tracking_api* tracking_api);
	void BeginStreaming(NodeId<local> whichNode) ;
	void EndStreaming(NodeId<local> whichNode);
	void RequestCompass(NodeId<local> whichNode);
	void RequestGravity(NodeId<local> whichNode);
	void RequestTracking(NodeId<local> whichNode) ;

	void OnTrackingUpdate(std::function<void(NodeId<local>, tracking_value)> handler);


private:
	tracking_api* m_api;
	std::function<void(NodeId<local>, tracking_value)> m_trackingQuaternionCb;

	std::unordered_map<NodeId<local>, stream> m_streams;
};