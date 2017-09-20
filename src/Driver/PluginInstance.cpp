#include "stdafx.h"
#include "PluginInstance.h"

#include <boost/type_index.hpp>
#include <iostream>
#include "PluginAPI.h"
#include "DeviceContainer.h"


PluginInstance::PluginInstance(boost::asio::io_service& io,  std::string fileName) :
	m_fileName(fileName), 
	m_loaded{ false },
	m_pluginFunctions{},
	m_pluginRegisterFunction{},
	m_apis(),
	m_eventHandler(),
	m_io(io),
	m_logger(boost::log::keywords::channel = "plugin")
	
{
	boost::filesystem::path pluginPath(m_fileName);
	

	m_logger.add_attribute("Plugin", boost::log::attributes::constant<std::string>(pluginPath.filename().string()));
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
			LOG_ERROR() << "Runtime error in plugin " << m_displayName << ": " << err.what();
			return false;
		}
	}

	bool result = true;
	return result;
}

bool PluginInstance::Link()
{
	boost::system::error_code loadFailure;

	m_dll = std::make_unique<boost::dll::shared_library>(m_fileName, boost::dll::load_mode::append_decorations | boost::dll::load_mode::load_with_altered_search_path, loadFailure);
	
	if (loadFailure) {
		LOG_ERROR() << "Failed to load " << m_fileName << " (.dll/.so): " << loadFailure.message();
		if (loadFailure.value() == 126) {
			LOG_INFO() << "--> Common causes besides not being able to find the dll:";
			LOG_INFO() << "--> 1) The DLL name does not correspond to the manifest name (Plugin_manifest.json => Plugin.dll)";
			LOG_INFO() << "--> 2) The DLL has additional dependencies which were not found";

		}
		return false;
	}


	if (!tryLoad(m_dll, "nsvr_plugin_register", m_pluginRegisterFunction)) {
		LOG_ERROR() << "Couldn't locate required symbol nsvr_plugin_register in " << m_fileName;

		return false;
	}
	else {
		LOG_INFO() << "Loaded " << m_fileName;
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
	m_io.post([&lg = m_logger, level, cmp = std::string(component), msg = std::string(message)]() {
		//plogger::get().add_attribute("Component", boost::log::attributes::constant<std::string>(cmp));
		lg.add_attribute("Component", boost::log::attributes::mutable_constant<std::string>(cmp));

		BOOST_LOG_SEV(lg, level) << msg;
	});
}

void PluginInstance::setDispatcher(std::unique_ptr<PluginEventSource> dispatcher)
{
	m_eventHandler = std::move(dispatcher);
}

int PluginInstance::GetWorkingDirectory(nsvr_directory* outDir)
{
	std::fill(outDir->path, outDir->path + 500, 0);
	//this may not correspond to the dll location, but for now it does, so we use that
	boost::filesystem::path dllPath(m_fileName);
	std::string path = dllPath.parent_path().string();

	std::copy(path.begin(), path.end(), outDir->path);
	return 1;
}

