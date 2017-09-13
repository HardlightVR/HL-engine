#include "stdafx.h"
#include "TrackingStream.h"




TrackingStream::TrackingStream(nsvr_node_id id, tracking_api*  api)
	: m_api(api)
	, m_id(id)
{
	m_api->submit_beginstreaming(reinterpret_cast<nsvr_tracking_stream*>(this), m_id);
}

TrackingStream::~TrackingStream()
{
	m_api->submit_endstreaming(m_id);
}

void TrackingStream::deliver(nsvr_quaternion* q)
{
	//Note that there is no queue, tracking is delivered directly from the given plugin into shared memory 
	//we need synchronization for this, of course. Do we have it? I don't think so
	//todo: verify we have sync
	Signal(m_id, q);
}

