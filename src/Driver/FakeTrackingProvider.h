#pragma once
#include "TrackingProvider.h"
#include "PluginApis.h"
#include <memory>
class FakeTrackingProvider : public TrackingProvider {
public:
	FakeTrackingProvider();
	void Augment(tracking_api* api);
	void BeginStreaming(NodeId<local> whichNode) override;
	void EndStreaming(NodeId<local> whichNode) override;
	void OnUpdate(std::function<void(NodeId<local>, nsvr_quaternion*)> handler) override;
private:

};

class FakeTrackingBuilder {
public:
	std::unique_ptr<FakeTrackingProvider> Build();
};