#pragma once
#include "pevent.h"
#include <unordered_map>
#include <vector>
#include "pevent.h"
#include <unordered_map>
#include <boost/asio/io_service.hpp>
class PluginEventHandler {
public:
	PluginEventHandler(boost::asio::io_service& io);
	using EventHandler = std::function<void(const nsvr::pevents::device_event&)>;
	void Raise(nsvr::pevents::device_event event);

	void Subscribe(nsvr_device_event_type type, EventHandler);
private:
	std::unordered_map<nsvr_device_event_type, std::vector<EventHandler>> m_subscribers;
	//should use a synchronized queue for event delivery instead of delivering in the plugins thread

	boost::asio::io_service& m_io;

};