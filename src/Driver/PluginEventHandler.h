#pragma once
#include "pevent.h"
#include <unordered_map>
#include <vector>
#include "pevent.h"
#include <unordered_map>
#include <boost/asio/io_service.hpp>
#include <boost/signals2.hpp>
class PluginEventHandler {
public:
	PluginEventHandler(boost::asio::io_service& io);
	using EventHandler = std::function<void(uint64_t)>;

	using EventSignal = boost::signals2::signal<void(uint64_t)>;
	void Raise(nsvr_device_event_type type, uint64_t id);

	void Subscribe(nsvr_device_event_type type, EventSignal::slot_type);
private:
	std::unordered_map<nsvr_device_event_type, EventSignal> m_subscribers;
	//should use a synchronized queue for event delivery instead of delivering in the plugins thread
	EventSignal m_signal;
	boost::asio::io_service& m_io;

};