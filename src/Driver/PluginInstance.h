#pragma once
#include <string>
#include <memory>
#include <boost/dll.hpp>
#include <boost/log/trivial.hpp>
#include "PluginAPI.h"

#include "CoreFacade.h"
#include "PluginCapabilities.h"
#include "PluginEventHandler.h"
class PluginInstance
{
public:


	PluginInstance(std::string fileName);
	~PluginInstance();
	
	bool ParseManifest();
	bool Link();
	bool Load();

	bool Configure();


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


	PluginCapabilities m_registry;
	PluginEventHandler m_eventHandler;
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


template<class THapticType>
void PluginInstance::Dispatch(const std::string& region, const std::string& iface, const THapticType* input) {
	std::vector<PluginInstance::ClientData> callbacks = getProviders(region, iface);
	for (const PluginInstance::ClientData& clientData : callbacks) {
		clientData.Callback(clientData.Data, region.c_str(), iface.c_str(), AS_TYPE(const nsvr_event_generic, input));
	}
}

template<class THapticType>
inline void PluginInstance::Broadcast(const std::string& iface,  const THapticType * input)
{
	for (const auto& region : m_interfaces) {
		std::vector<PluginInstance::ClientData> callbacks = getProviders(region.first, iface);
		for (const auto& clientData : callbacks) {
			clientData.Callback(clientData.Data, region.first.c_str(), iface.c_str(), AS_TYPE(const nsvr_event_generic, input));
		}
	}
	
}


