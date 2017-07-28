#pragma once
#include "pevent.h"
#include <unordered_map>
#include <vector>
class PluginEventHandler {
public:
	using EventHandler = std::function<void(const nsvr::pevents::device_event&)>;
	void Raise(const nsvr::pevents::device_event& event);

	void Subscribe(nsvr_device_event_type type, EventHandler);
private:
	std::unordered_map<nsvr_device_event_type, std::vector<EventHandler>> m_subscribers;
};