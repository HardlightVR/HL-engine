#pragma once
#include <string>
#include <memory>
#include <boost/dll.hpp>
#include <boost/log/trivial.hpp>
#include "PluginAPI.h"

#include "CoreFacade.h"
#include "PluginApis.h"
#include "PluginEventSource.h"

#include "DriverConfigParser.h"

class DeviceContainer;

class PluginInstance
{
public:


	PluginInstance(boost::asio::io_service& io, std::string fileName, DeviceContainer& d);
	~PluginInstance();
	
	bool ParseManifest();
	bool Link();
	bool Load();

	bool Configure();

	bool run_update_loop_once(uint64_t dt);

	bool Unload();
	bool IsLoaded() const;
	std::string GetFileName() const;
	std::string GetDisplayName() const;


	PluginInstance(const PluginInstance&) = delete;
	const PluginInstance& operator=(const PluginInstance&) = delete;
	PluginInstance(PluginInstance&&) = delete;

	

private:
	std::unique_ptr<boost::dll::shared_library> m_dll;


	typedef std::function<int(nsvr_plugin_api*)> plugin_registration_t;
	plugin_registration_t m_pluginRegisterFunction;

	nsvr_plugin_api m_pluginFunctions;
	nsvr_plugin* m_pluginPointer;

	std::string m_displayName;
	std::string m_fileName;
	bool m_loaded;


	DeviceContainer& m_deviceContainer;
	Parsing::ManifestDescriptor m_descriptor;
	PluginApis m_apis;
	PluginEventSource m_eventHandler;
	CoreFacade m_facade;


};

template<class TFunc>
bool tryLoad(std::unique_ptr<boost::dll::shared_library>& lib, const std::string& symbol, std::function<TFunc>& result) {
	try {
		result = lib->get<TFunc>(symbol);
		return result ? true : false;
	}
	catch (const boost::system::system_error&) {
		BOOST_LOG_TRIVIAL(warning) << "[PluginInstance] Unable to find function named " << symbol << '\n';
		return false;
	}

}




