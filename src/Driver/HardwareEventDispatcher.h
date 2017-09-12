#pragma once
#include <stdint.h>
#include <boost/asio/io_service.hpp>
#include <boost/signals2.hpp>
#include <unordered_map>
#include "PluginAPI.h"
#include "PluginEventSource.h"
#include "PluginEventSink.h"

///
/// This is a simple event dispatcher which forms a communication bridge
/// between an individual plugin and the core. You may subscribe to a plugin event
/// using Subscribe. Events are delivered asynchronously, on the io thread.
/// 
class HardwareEventDispatcher : public PluginEventSource, public PluginEventSink {
public:
	HardwareEventDispatcher(boost::asio::io_service& io);

	using EventSignal = boost::signals2::signal<void(nsvr_device_id)>;
	void Raise(nsvr_device_event_type type, nsvr_device_id id) override;

	void Subscribe(nsvr_device_event_type type, EventSignal::slot_type) override;
private:
	std::unordered_map<nsvr_device_event_type, EventSignal> m_subscribers;
	boost::asio::io_service& m_io;
};