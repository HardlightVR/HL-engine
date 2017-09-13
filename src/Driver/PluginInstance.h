#pragma once
#include <string>
#include <memory>
#include <boost/dll.hpp>
#include <boost/log/trivial.hpp>
#include "PluginAPI.h"

#include "PluginApis.h"
#include "PluginEventSource.h"
#include "DriverConfigParser.h"

class DeviceContainer;

class PluginInstance
{
public:


	PluginInstance(std::unique_ptr<PluginEventSource> dispatcher, std::string fileName);
	~PluginInstance();
	
	bool ParseManifest();
	bool Link();
	bool Instantiate();

	bool Configure();

	bool tick_once(uint64_t dt);

	bool Unload();
	bool IsLoaded() const;
	std::string GetFileName() const;
	std::string GetDisplayName() const;


	PluginInstance(const PluginInstance&) = delete;
	const PluginInstance& operator=(const PluginInstance&) = delete;
	PluginInstance(PluginInstance&&) = delete;

	PluginApis& apis();

	template<typename InternalApi, typename ExternalApi>
	void RegisterPluginApi(ExternalApi* api);

	void RaiseEvent(nsvr_device_event_type type, nsvr_device_id id);

private:
	std::unique_ptr<boost::dll::shared_library> m_dll;


	typedef std::function<int(nsvr_plugin_api*)> plugin_registration_t;
	plugin_registration_t m_pluginRegisterFunction;

	nsvr_plugin_api m_pluginFunctions;
	nsvr_plugin* m_pluginPointer;

	std::string m_displayName;
	std::string m_fileName;
	bool m_loaded;

	PluginApis m_apis;
	std::unique_ptr<PluginEventSource> m_eventHandler;


};

template<class TFunc>
bool tryLoad(std::unique_ptr<boost::dll::shared_library>& lib, const std::string& symbol, std::function<TFunc>& result) {
	try {
		result = lib->get<TFunc>(symbol);
		return result ? true : false; //This looks dumb. But I did it because result wasn't being implicitly converted to bool
		//it still looks dumb. 
		//return (bool)result;
	}
	catch (const boost::system::system_error&) {
		BOOST_LOG_TRIVIAL(warning) << "[PluginInstance] Unable to find function named " << symbol << '\n';
		return false;
	}

}

template<typename InternalApi, typename ExternalApi>
inline void PluginInstance::RegisterPluginApi(ExternalApi * api)
{
	m_apis.Register<InternalApi>(api);
}



//TODO put corecafade back in the plugin

