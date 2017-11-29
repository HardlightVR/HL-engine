#pragma once

#include "TrackingProvider.h"
#include "PluginApis.h"
class HardwareTracking : public TrackingProvider {

public:

	struct stream {
		std::function<void(nsvr_quaternion* quat)> deliver;
		std::function<void(nsvr_vector3* vec)> deliverCompass;
		std::function<void(nsvr_vector3* vec)> deliverGravity;
	};
	HardwareTracking(tracking_api* tracking_api);
	void BeginStreaming(NodeId<local> whichNode) override;
	void EndStreaming(NodeId<local> whichNode) override;
	void RequestCompass(NodeId<local> whichNode) override;
	void RequestGravity(NodeId<local> whichNode) override;
	void RequestTracking(NodeId<local> whichNode) override;
	void OnTrackingQuaternion(std::function<void(NodeId<local>, nsvr_quaternion *)> handler) override;
	void OnTrackingCompass(std::function<void(NodeId<local>, nsvr_vector3*)> handler) override;
	void OnTrackingGravity(std::function<void(NodeId<local>, nsvr_vector3*)> handler) override;

private:
	tracking_api* m_api;
	std::function<void(NodeId<local>, nsvr_quaternion*)> m_trackingQuaternionCb;
	std::function<void(NodeId<local>, nsvr_vector3*)> m_trackingCompassCb;
	std::function<void(NodeId<local>, nsvr_vector3*)> m_trackingGravityCb;
	std::unordered_map<NodeId<local>, stream> m_streams;
};