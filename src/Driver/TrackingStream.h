#pragma once
#include "PluginAPI.h"
#include "PluginApis.h"
#include <boost/signals2.hpp>

//raii style?
class TrackingStream {
public:
	TrackingStream(nsvr_node_id id, tracking_api* api);
	~TrackingStream();
	boost::signals2::signal<void(nsvr_node_id, nsvr_quaternion*)> Signal;
	void deliver(nsvr_quaternion* q);
private:
	nsvr_node_id m_id;
	tracking_api* m_api;
};





