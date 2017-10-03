#pragma once
#include "PluginAPI.h"
#include "DeviceIds.h"
#include <functional>

class TrackingProvider {
public:
	virtual void BeginStreaming(NodeId<local> whichNode) = 0;
	virtual void EndStreaming(NodeId<local> whichNode) = 0;
	virtual void OnUpdate(std::function<void(NodeId<local>, nsvr_quaternion*)> handler) = 0;

};