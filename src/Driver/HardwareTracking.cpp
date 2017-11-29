#include "stdafx.h"
#include "HardwareTracking.h"

HardwareTracking::HardwareTracking(tracking_api * tracking_api)
	: m_api(tracking_api)
{
}

void HardwareTracking::BeginStreaming(NodeId<local> whichNode)
{
	m_streams[whichNode] = stream { 
		[=](nsvr_quaternion* quat) {
			m_trackingQuaternionCb(whichNode, quat);
		},
		[=] (nsvr_vector3* compass) {
			m_trackingCompassCb(whichNode, compass);
		},
		[=](nsvr_vector3* gravity) {
			m_trackingGravityCb(whichNode, gravity);
		}
	};

	m_api->submit_beginstreaming(reinterpret_cast<nsvr_tracking_stream*>(&m_streams.at(whichNode)), whichNode.value);
}

void HardwareTracking::EndStreaming(NodeId<local> whichNode)
{
	m_api->submit_endstreaming(whichNode.value);
}

void HardwareTracking::OnTrackingQuaternion(std::function<void(NodeId<local>, nsvr_quaternion*)> handler)
{
	m_trackingQuaternionCb = std::move(handler);
}

void HardwareTracking::OnTrackingCompass(std::function<void(NodeId<local>, nsvr_vector3*)> handler)
{
	m_trackingCompassCb = std::move(handler);
}

void HardwareTracking::OnTrackingGravity(std::function<void(NodeId<local>, nsvr_vector3*)> handler)
{
	m_trackingGravityCb = std::move(handler);
}

