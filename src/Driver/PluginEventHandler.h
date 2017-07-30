#pragma once
#include "pevent.h"
#include <unordered_map>
#include <vector>
#include <boost/lockfree/spsc_queue.hpp>
class PluginEventHandler {
public:
	using EventHandler = std::function<void(const nsvr::pevents::device_event&)>;
	void Raise(const nsvr::pevents::device_event& event);

	void Subscribe(nsvr_device_event_type type, EventHandler);
private:
	std::unordered_map<nsvr_device_event_type, std::vector<EventHandler>> m_subscribers;
	//should use a synchronized queue for event delivery instead of delivering in the plugins thread
};