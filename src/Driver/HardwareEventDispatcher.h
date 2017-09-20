#pragma once
#include <stdint.h>
#include <boost/asio/io_service.hpp>

#include <unordered_map>
#include "PluginAPI.h"
#include "PluginEventSource.h"


#include "PluginApis.h"

#include <boost/signals2.hpp>
#include <boost/signals2/signal_type.hpp>
#include "DriverConfigParser.h"
class PluginInstance;
///
/// This is a simple event dispatcher which forms a communication bridge
/// between an individual plugin and the core. You may subscribe to a plugin event
/// using Subscribe. Events are delivered asynchronously, on the io thread.
/// 
class HardwareEventDispatcher : public PluginEventSource {
public:
	HardwareEventDispatcher(boost::asio::io_service& io);



	void Raise(nsvr_device_event_type type, nsvr_device_id id, PluginInstance& instance) override;


	using DeviceConnected = boost::signals2::signal<void(nsvr_device_id, PluginApis& apis)>;
	using DeviceDisconnected = boost::signals2::signal<void(nsvr_device_id)>;

	void OnDeviceConnected(DeviceConnected::slot_type);
	void OnDeviceDisconnected(DeviceDisconnected::slot_type);

	
private:
	DeviceConnected m_onDeviceCreated;
	DeviceDisconnected m_onDeviceDisconnected;

	boost::asio::io_service& m_io;
};