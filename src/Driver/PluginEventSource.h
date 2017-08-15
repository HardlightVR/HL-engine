#pragma once

#include <stdint.h>
#include <boost/asio/io_service.hpp>
#include <boost/signals2.hpp>
#include <unordered_map>
#include "PluginAPI.h"

///
/// This is a simple event dispatcher which forms a communication bridge
/// between an individual plugin and the core. You may subscribe to a plugin event
/// using Subscribe. Events are delivered asynchronously, on the io thread.
/// 
class PluginEventSource {
public:
	PluginEventSource(boost::asio::io_service& io);

	using EventSignal = boost::signals2::signal<void(uint64_t)>;
	void Raise(nsvr_device_event_type type, uint64_t id);

	void Subscribe(nsvr_device_event_type type, EventSignal::slot_type);
private:
	std::unordered_map<nsvr_device_event_type, EventSignal> m_subscribers;
	EventSignal m_signal;
	boost::asio::io_service& m_io;
};