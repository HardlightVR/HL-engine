#pragma once
#include <string>
#include <unordered_map>
#include "../include/PluginAPI.h"

#include <chrono>
#include <boost/function.hpp>
#include <boost/dll.hpp>

#include "events/briefTaxel.h"
#include "events/lastingTaxel.h"
#include <boost/log/trivial.hpp>

#include "HardwareDataModel.h"



typedef struct nsvr_callback {
	void* callback;
	nsvr_core_ctx* context;
} nsvr_callback;



typedef struct NSVR_Configuration_s {

	std::unordered_map<std::string, nsvr_callback> Callbacks;

} NSVR_Configuration;


struct FunctionTable {

};
class PluginInstance
{
public:


	PluginInstance(std::string fileName, HardwareDataModel& model);
	~PluginInstance();
	bool ParseManifest();
	bool Link();
	bool Load();

	bool Configure();
;

	bool Unload();
	bool IsLoaded() const;
	std::string GetFileName() const;
	std::string GetDisplayName() const;


	PluginInstance(const PluginInstance&) = delete;
	const PluginInstance& operator=(const PluginInstance&) = delete;
	PluginInstance(PluginInstance&&) = delete;

	



private:

	typedef std::function<int(NSVR_Plugin**)> plugin_creator_t;
	typedef std::function<int(NSVR_Plugin**)> plugin_destructor_t;
	typedef std::function<int(NSVR_Plugin*, nsvr_core_ctx*)> plugin_configure_t;
	std::unique_ptr<boost::dll::shared_library> m_lib;
	
	NSVR_Plugin* m_rawPtr;

	plugin_creator_t m_creator;
	plugin_destructor_t m_destructor;
	plugin_configure_t m_configure;

	std::string m_displayName;
	std::string m_fileName;
	bool m_loaded;


	HardwareDataModel& m_model;




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


