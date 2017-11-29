#include "stdafx.h"
#include "HardwareTracking.h"

HardwareTracking::HardwareTracking(tracking_api * tracking_api)
	: m_api(tracking_api)
{
}

void HardwareTracking::BeginStreaming(NodeId<local> whichNode)
{
	m_streams[whichNode] = stream { 
		[=](tracking_value quat) {
			m_trackingQuaternionCb(whichNode, quat);
		}
	};

	m_api->submit_beginstreaming(reinterpret_cast<nsvr_tracking_stream*>(&m_streams.at(whichNode)), whichNode.value);
}

void HardwareTracking::EndStreaming(NodeId<local> whichNode)
{
	m_api->submit_endstreaming(whichNode.value);
}

void HardwareTracking::RequestCompass(NodeId<local> whichNode)
{
	m_api->submit_getcompass(whichNode.value);
}

void HardwareTracking::RequestGravity(NodeId<local> whichNode)
{
	m_api->submit_getgravity(whichNode.value);
}

void HardwareTracking::RequestTracking(NodeId<local> whichNode)
{
	m_api->submit_pollonce(whichNode.value);
}

void HardwareTracking::OnTrackingUpdate(std::function<void(NodeId<local>, tracking_value)> handler)
{
	m_trackingQuaternionCb = std::move(handler);
}
