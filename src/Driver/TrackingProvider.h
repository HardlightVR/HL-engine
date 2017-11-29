#pragma once
#include "PluginAPI.h"
#include "DeviceIds.h"
#include <functional>

class TrackingProvider {
public:
	virtual void BeginStreaming(NodeId<local> whichNode) = 0;
	virtual void EndStreaming(NodeId<local> whichNode) = 0;
	virtual void OnTrackingQuaternion(std::function<void(NodeId<local>, nsvr_quaternion*)> handler) = 0;
	virtual void OnTrackingCompass(std::function<void(NodeId<local>, nsvr_vector3*)> handler) = 0;
	virtual void OnTrackingGravity(std::function<void(NodeId<local>, nsvr_vector3*)> handler) = 0;

};