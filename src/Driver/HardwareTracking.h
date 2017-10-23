#pragma once

#include "TrackingProvider.h"
#include "PluginApis.h"
class HardwareTracking : public TrackingProvider {

public:

	struct stream {
		std::function<void(nsvr_quaternion* quat)> deliver;
	};
	HardwareTracking(tracking_api* tracking_api);
	void BeginStreaming(NodeId<local> whichNode) override;
	void EndStreaming(NodeId<local> whichNode) override;
	void OnUpdate(std::function<void(NodeId<local>, nsvr_quaternion *)> handler) override;
	~HardwareTracking() {
		std::cout << "TRACKING DESTRUCTOR\n";
	}
private:
	tracking_api* m_api;
	std::function<void(NodeId<local>, nsvr_quaternion*)> m_callback;
	std::unordered_map<NodeId<local>, stream> m_streams;
};