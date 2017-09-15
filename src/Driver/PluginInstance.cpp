#include "stdafx.h"
#include "PluginInstance.h"

#include <boost/type_index.hpp>
#include <iostream>
#include "PluginAPI.h"
#include "DeviceContainer.h"
#include "logger.h"
PluginInstance::PluginInstance(boost::asio::io_service& io, std::unique_ptr<PluginEventSource> dispatcher, std::string fileName) :
	m_fileName(fileName), 
	m_loaded{ false },
	m_pluginFunctions{},
	m_pluginRegisterFunction{},
	m_apis(),
	m_eventHandler(std::move(dispatcher)),
	m_io(io)
	
{


}




PluginInstance::~PluginInstance()
{
}





//Precondition: linked successfully
//does nothing if already loaded. May want to reinstantiate instead. We'll see.
bool PluginInstance::Instantiate()
{
	if (m_loaded) {
		return true;
	}


	if (m_pluginRegisterFunction) {
		m_pluginRegisterFunction(&m_pluginFunctions);

		m_loaded = m_pluginFunctions.init(&m_pluginPointer);
		
	}	
	
	return m_loaded;
	
	
}



//precondition: successfully loaded
bool PluginInstance::Configure()
{
	if (m_pluginFunctions.configure) {
		return m_pluginFunctions.configure(m_pluginPointer, reinterpret_cast<nsvr_core*>(this));
	}
	
	return false;
}

bool PluginInstance::tick_once(uint64_t dt)
{
	if (auto api = m_apis.GetApi<updateloop_api>()) {
		try {
			api->submit_update(dt);
		}
		catch (const std::runtime_error& err) {
		//	BOOST_LOG_TRIVIAL(error) << "Runtime ERROR in plugin " << m_displayName << ": " << err.what();
			return false;
		}
	}

	bool result = true;
	return result;
}

bool PluginInstance::Link()
{
	boost::system::error_code loadFailure;

	m_dll = std::make_unique<boost::dll::shared_library>(m_fileName, boost::dll::load_mode::append_decorations, loadFailure);
	
	if (loadFailure) {
	//	BOOST_LOG_TRIVIAL(error) << "Failed to load " << m_fileName << " (.dll/.so): " << loadFailure.message();
		if (loadFailure.value() == 126) {
		//	BOOST_LOG_TRIVIAL(error) << "Common causes:";
		//	BOOST_LOG_TRIVIAL(error) << "1) The DLL name does not correspond to the manifest name (Plugin_manifest.json => Plugin.dll)";
		//	BOOST_LOG_TRIVIAL(error) << "2) The DLL has additional dependencies which were not found";

		}
		return false;
	}

	//BOOST_LOG_TRIVIAL(info) << "Loaded " << m_fileName;

	if (!tryLoad(m_dll, "nsvr_plugin_register", m_pluginRegisterFunction)) {
		return false;
	}
	
	return true;

}

//precondition: linked successfully
bool PluginInstance::Unload()
{
	bool result = false;
	if (m_pluginFunctions.free) {
		result = m_pluginFunctions.free(m_pluginPointer);
		m_pluginPointer = nullptr;
	}
	m_dll.reset();

	m_loaded = false;
	return result;
}

bool PluginInstance::IsLoaded() const
{
	return m_loaded;
}

std::string PluginInstance::GetFileName() const
{
	return m_fileName;
}

std::string PluginInstance::GetDisplayName() const
{
	return m_displayName;
}


PluginApis & PluginInstance::apis()
{
	return m_apis;
}

void PluginInstance::RaiseEvent(nsvr_device_event_type type, nsvr_device_id id)
{
	m_eventHandler->Raise(type, id, *this);
}

void PluginInstance::Log(nsvr_loglevel level, const char * component, const char * message)
{
	m_io.post([level, cmp = std::string(component), msg = std::string(message), filename = m_fileName]() {
		BOOST_LOG_SEV(sclogger::get(), level) << "[" << filename << "]:[" << cmp << "] " << msg;
	});
}

