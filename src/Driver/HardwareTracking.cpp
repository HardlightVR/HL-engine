#include "stdafx.h"
#include "HardwareTracking.h"

HardwareTracking::HardwareTracking(tracking_api * tracking_api)
	: m_api(tracking_api)
{
}

void HardwareTracking::BeginStreaming(NodeId<local> whichNode)
{
	m_streams[whichNode] = stream{ [=](nsvr_quaternion* quat) {
		m_callback(whichNode, quat);
	}};

	m_api->submit_beginstreaming(reinterpret_cast<nsvr_tracking_stream*>(&m_streams.at(whichNode)), whichNode.value);
}

void HardwareTracking::EndStreaming(NodeId<local> whichNode)
{
	m_api->submit_endstreaming(whichNode.value);
}

void HardwareTracking::OnUpdate(std::function<void(NodeId<local>, nsvr_quaternion*)> handler)
{
	m_callback = std::move(handler);
}

