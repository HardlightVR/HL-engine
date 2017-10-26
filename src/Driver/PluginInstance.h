#pragma once
#include <string>
#include <memory>
#include <boost/dll.hpp>
#include "PluginAPI.h"

#include "PluginApis.h"
#include "PluginEventSource.h"
#include "DriverConfigParser.h"
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include "logger.h"
#include "DeviceIds.h"
#include "FakeResources.h"
#include "DeviceDescriptor.h"

class DeviceContainer;

class PluginInstance : public std::enable_shared_from_this<PluginInstance>
{
public:
	struct DeviceResources {
		DeviceId<local> id; //why using this??
		boost::optional<DeviceDescriptor> deviceDescriptor;
		boost::optional<Parsing::BodyGraphDescriptor> bodygraphDescriptor;
		std::unique_ptr<FakeInterface<bodygraph_api>> bodygraph;
		std::unique_ptr<FakeInterface<device_api>> discoverer;
		std::unique_ptr<FakeInterface<tracking_api>> tracking;
		std::unique_ptr<FakeInterface<playback_api>> playback;
		std::unique_ptr<FakeInterface<waveform_api>> waveformHaptics;
		std::unique_ptr<FakeInterface<buffered_api>> bufferedHaptics;
	};

	~PluginInstance();

	using DeviceResourceBundle = std::unique_ptr<DeviceResources>;

	PluginInstance(boost::asio::io_service& io, std::string fileName, uint32_t id);
	
	bool Link();
	bool Instantiate();

	bool Configure();

	bool tick_once(uint64_t dt);

	bool Unload();
	bool IsLoaded() const;
	std::string GetFileName() const;
	uint32_t GetId() const;
	PluginInstance(const PluginInstance&) = delete;
	const PluginInstance& operator=(const PluginInstance&) = delete;
	PluginInstance(PluginInstance&&) = delete;

	PluginApis& apis();

	template<typename InternalApi, typename ExternalApi>
	void RegisterPluginApi(ExternalApi* api);

	void RaiseEvent(nsvr_device_event_type type, nsvr_device_id id);
	void LogAsync(nsvr_severity level, const char * component, const char * message);

	void setDispatcher(std::unique_ptr<PluginEventSource> dispatcher);
	void addDeviceResources(DeviceResourceBundle resources);

	//Non-const.. we could return the unique_ptr, then modify it in DeviceBuilder etc., then return it back.
	//Or we could pass a non-owning pointer to it.
	DeviceResources* resources();
	void PluginInstance::LogNow(nsvr_severity level, const std::string& component, const std::string& message);
private:
	std::string m_fileName;
	bool m_loaded;
	uint32_t m_id;

	boost::asio::io_service& m_io;
	std::shared_ptr<my_logger> m_logger;
	DeviceResourceBundle m_resources;
	nsvr_plugin_api m_pluginFunctions;
	nsvr_plugin* m_pluginPointer;

	std::unique_ptr<PluginEventSource> m_eventHandler;
	PluginApis m_apis;

	using plugin_registration_t = std::function<int(nsvr_plugin_api*)>;
	plugin_registration_t m_pluginRegisterFunction;

	

	std::unique_ptr<boost::dll::shared_library> m_dll;



public:
	int GetWorkingDirectory(nsvr_directory* outDir);
};

template<class TFunc>
bool tryLoad(std::unique_ptr<boost::dll::shared_library>& lib, const std::string& symbol, std::function<TFunc>& result) {
	try {
		result = lib->get<TFunc>(symbol);
		return (bool) result;
	}
	catch (const boost::system::system_error&) {
		BOOST_LOG_SEV(clogger::get(), nsvr_severity_warning) << "[PluginInstance] Unable to find function named " << symbol << '\n';
		return false;
	}

}

template<typename InternalApi, typename ExternalApi>
inline void PluginInstance::RegisterPluginApi(ExternalApi * api)
{
	m_apis.Register<InternalApi>(api);
}




